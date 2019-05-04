#include <stdint.h>
#include <stdio.h>
#include <string.h>
//#include <vector>

#include "board.h"
#include "clock_config.h"

#include "fsl_eeprom.h"
#include "fsl_nor_flash.h"

#include "../../Raspberry/PanPreset.h"

extern nor_handle_t norHandle;

struct presetnames_t {
	unsigned char names[PRESET_COUNT * PRESET_NAME_LENGTH];
};

extern presetnames_t presetnames;

uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

uint32_t crc32(const void *data, size_t n_bytes) {
  uint32_t crc = 0;
  static uint32_t table[0x100];
  if(!*table)
    for(size_t i = 0; i < 0x100; ++i)
      table[i] = crc32_for_byte(i);
  for(size_t i = 0; i < n_bytes; ++i)
    crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;
  return crc;
}

struct preset_header_t {
  uint32_t preset_checksum;
  uint32_t page0_checksum;
  uint32_t generation;
  uint16_t preset_id;
  uint16_t pad;
};

struct preset_t {
  preset_t() { memset(this, 0, sizeof(preset_t)); }

  preset_header_t header;
  PanPreset_t data;
};

struct index_entry_t {
  uint32_t generation;
  uint16_t page;
  uint16_t valid;
  uint8_t name[16];
};

#define align(value, step) (((value) + (step) - 1) & ~(step-1))

#define MEMSIZE (8*1048576)
#define BLOCKSIZE 256
#define SECTORSIZE 4096

const int preset_size = align(sizeof(preset_t), SECTORSIZE);
const int preset_count = MEMSIZE / preset_size;

int LoadScanIndex()
{
  int index = 0;

	memcpy(&index, (void*)FSL_FEATURE_EEPROM_BASE_ADDRESS, sizeof(int));
	if (index >= preset_count) index = 0;

/*  FILE *f = fopen("store.index", "rb");
  if (!f) return 0;
  fread(&index, 4, 1, f);
  fclose(f);*/
  return index;
}

#define EEPROM_PAGE_SIZE (128)
uint8_t eeprom_page[EEPROM_PAGE_SIZE];

void StoreScanIndex(int index)
{
	//int pagecount = 1;//(sizeof(struct flashcontent_t) + (EEPROM_PAGE_SIZE-1)) / EEPROM_PAGE_SIZE;
	memset(eeprom_page, 0, EEPROM_PAGE_SIZE);
	memcpy(eeprom_page, &index, 4);

//for (int i = 0; i < pagecount; i++) {
	EEPROM_WritePage(EEPROM, 0, (uint32_t *)eeprom_page);

/*  FILE *f = fopen("store.index", "wb");
  if (!f) return;
  fwrite(&index, 4, 1, f);
  fclose(f);*/
}

class Store
{
public:
  explicit Store(int size);
  ~Store();

  int Size() const { return MEMSIZE; }

  void Read(int address, uint8_t* buffer);
  void Write(int address, uint8_t* buffer);
  void Erase(int address);

  //std::vector<uint8_t> data_;
};

Store::Store(int size)// : data_(size)
{
//  memset(&data_[0], 255, size);
/*  FILE *f = fopen("store.dat", "rb");
  if (!f) return;
  fread(&data_[0], size, 1, f);
  fclose(f);*/
}

Store::~Store()
{
/*  FILE *f = fopen("store.dat", "wb");
  if (!f) return;
  fwrite(&data_[0], Size(), 1, f);
  fclose(f);*/
}
  
void Store::Read(int address, uint8_t* buffer)
{
//  memcpy(buffer, &data_[address], BLOCKSIZE);
	address += FSL_FEATURE_SPIFI_START_ADDR;
	int status = Nor_Flash_Read(&norHandle, address, buffer, BLOCKSIZE);
	if (status != kStatus_Success)
	{
		printf("\r\n***NOR Flash Address %x Read Failed!***\r\n", address);
		return;
	}
}

void Store::Write(int address, uint8_t* buffer)
{
  //memcpy(&data_[address], buffer, BLOCKSIZE);
	address += FSL_FEATURE_SPIFI_START_ADDR;
	int status = Nor_Flash_Page_Program(&norHandle, address, buffer);
	if (status != kStatus_Success)
	{
		printf("\r\n***NOR Flash Address %x Program Failed!***\r\n", address);
		return;
	}
}

void Store::Erase(int address)
{
  //memset(&data_[address], 255, SECTORSIZE);
	address += FSL_FEATURE_SPIFI_START_ADDR;
	int status = Nor_Flash_Erase_Block(&norHandle, address, SECTORSIZE);
	if (status != kStatus_Success)
	{
		printf("\r\n***NOR Flash Address %x Erase Failed!***\r\n", address);
		return;
	}
}

class Index
{
  uint8_t buf[BLOCKSIZE];
public:
  Index(Store& s) : s_(s), scanindex_(0) { }

  void Init()
  {
	  scanindex_ = LoadScanIndex();
	  Reindex();
  }

  void Reindex() {
    preset_t preset;

    for (int i = 0; i < preset_count; i++) {

      s_.Read(i * preset_size, buf);
      if (IsEmptyBlock(buf)) {
		SetEmptyPreset(i);
/*        if (IsEmptyPreset(i)) {
        }
        else {
          SetDirtyPreset(i);
        }*/
      }
      else if (VerifyPage0Checksum(buf)) {
        if (ReadPreset(i, &preset)) {
          SetValidPreset(i, &preset);
        }
        else {
          SetDirtyPreset(i);
        }
      }
      else {
        SetDirtyPreset(i);
      }
    } 
  }

#if 0
  void Refresh(void)
  {
    for (int i = 0; i < preset_count; i++) {
      if (index[i].valid == 2) {
        TouchPreset(i);
      }
    }
  }

  void TouchPreset(int id)
  {
    preset_t preset;
    
    ReadPreset(index[id].page, &preset);
    index[id].generation++;
    preset.header.generation = index[id].generation;
    UpdateChecksums(&preset);

    int freepage = GetFreePage();
    WritePreset(freepage, &preset);
  }
#endif

  void WipeDirty()
  {
    for (int i = 0; i < preset_count; i++) {
      if (state[i] == 1) {
	for (int j = 0; j < preset_size; j += SECTORSIZE) {
          s_.Erase(i * preset_size + j);
        }
        state[i] = 0;
      }
    }
  }

  void SavePreset(preset_t* preset)
  {
    WipeDirty();

    int freepage = GetFreePage();
    preset->header.generation = index[preset->header.preset_id].generation + 1;
    UpdateChecksums(preset);
    if (WritePreset(freepage, preset)) {
      SetValidPreset(freepage, preset);
    }
  }

  int LoadPreset(int preset_id, preset_t* preset)
  {
	  if (!index[preset_id].valid) {
		  return 0;
	  }

	  ReadPreset(index[preset_id].page, preset);

	  return 1;
  }

  int WritePreset(int page, preset_t* preset)
  {
    //printf("Write preset to page %d\n", page);
    uint8_t* p = (uint8_t*)preset;
    for (int i = 0; i < sizeof(preset_t); i += BLOCKSIZE) {
      if (sizeof(preset_t) - i < BLOCKSIZE) {
        memset(buf, 0, BLOCKSIZE);
        memcpy(buf, &p[i], sizeof(preset_t) - i);
        s_.Write(page * preset_size + i, buf); 
      }
      else {
        s_.Write(page * preset_size + i, &p[i]); 
      }
    }
    return 1;
  }

  int ReadPreset(int page, preset_t* preset)
  {
    uint8_t* p = (uint8_t*)preset;
    for (int i = 0; i < sizeof(preset_t); i += BLOCKSIZE) {
      if (sizeof(preset_t) - i < BLOCKSIZE) {
        s_.Read(page * preset_size + i, buf); 
        memcpy(&p[i], buf, sizeof(preset_t) - i);
      }
      else {
        s_.Read(page * preset_size + i, &p[i]); 
      }
    }
    return VerifyChecksum(preset);
  }

  int NextPage(int page)
  {
    int next = page + 1;
    if (next >= preset_count) next = 0;
    return next;
  }

  int GetFreePage()
  {
    int startindex = scanindex_;
    int foundpage = -1;

    int i = startindex;
    for (;
         NextPage(i) != startindex;
         i = NextPage(i))
    {
      if (state[i] == 0) {
        foundpage = i;
        break;
      }
    }

    scanindex_ = i;
    StoreScanIndex(scanindex_);

    return foundpage;
  }

  void UpdateChecksums(preset_t* preset)
  {
    preset->header.page0_checksum = crc32(&preset->header.generation, BLOCKSIZE - 8);
    preset->header.preset_checksum = crc32(&preset->header.page0_checksum, sizeof(preset_t) - 4);
    //printf("preset page0 %x crc %x\n", preset->header.page0_checksum, preset->header.preset_checksum);
  }

  int VerifyPage0Checksum(uint8_t* block)
  {
    uint32_t expected = crc32(&block[8], BLOCKSIZE - 8);
    uint32_t inblock = *(uint32_t*)&block[4];
   
    preset_t* pr = (preset_t*)block;
    expected = crc32(&pr->header.generation, BLOCKSIZE - 8); 
    //printf("block0 crc %x ?== %x in store\n", expected, inblock);
    
    return expected == inblock;
  }

  int VerifyChecksum(preset_t* preset)
  {
    uint32_t expected = crc32(&preset->header.page0_checksum, sizeof(preset_t) - 4);
    //printf("crc %x ?== %x in store\n", expected, preset->header.preset_checksum);
    return expected == preset->header.preset_checksum;
  }

  int IsEmptyBlock(uint8_t* block)
  {
    for (int i = 0; i < BLOCKSIZE; i++) {
      if (block[i] != 255) return 0;
    }
    return 1;
  }
  
  int IsEmptyPreset(int page)
  {
    for (int i = 0; i < preset_size; i += BLOCKSIZE) {
      s_.Read(page * preset_size + i, buf); 
      if (!IsEmptyBlock(buf)) return 0;
    }
    return 1;
  }

  int NewerGeneration(int newgen, int oldgen)
  {
    if (newgen < 0x40000000 && oldgen > 0x80000000) {
      return 2;
    }
    
    if (newgen > oldgen) {
      return 1;
    }

    return 0;
  }

  void SetValidPreset(int page, preset_t* preset)
  {
    int preset_id = preset->header.preset_id;
    int needrefresh = 0;

    if (index[preset_id].valid) {
      int newer = NewerGeneration(preset->header.generation, index[preset_id].generation);
      if (newer) {
        SetDirtyPreset(index[preset_id].page);
        if (newer == 2) {
          needrefresh = 1;
        }
      }
      else {
        // older
        SetDirtyPreset(page);
        return;
      }
    }
  
    //printf("page %d valid with preset %d\n", page, preset_id);
    state[page] = 2;
    index[preset_id].generation = preset->header.generation;
    index[preset_id].page = page;
    index[preset_id].valid = needrefresh ? 2 : 1;
    memcpy(&index[preset_id].name, preset->data.Name, 16);
    memcpy(&presetnames.names[preset_id * 16], preset->data.Name, 16);
  }

  void SetDirtyPreset(int page)
  {
    //printf("page %d dirty\n", page);
    state[page] = 1;
  }

  void SetEmptyPreset(int page)
  {
    state[page] = 0;
  }

  Store& s_;
  int scanindex_;
  int state[preset_count];
  index_entry_t index[preset_count];
};

Store s(MEMSIZE);
Index i(s);

preset_t tmp_preset;
void flash_savepreset(int presetid, PanPreset_t* preset)
{
	memcpy(&tmp_preset.data, preset, sizeof(PanPreset_t));
	tmp_preset.header.preset_id = presetid;
	i.SavePreset(&tmp_preset);
}

int flash_loadpreset(int presetid, PanPreset_t* preset)
{
	int status = i.LoadPreset(presetid, &tmp_preset);
	if (!status) {
		return 0;
	}

	memcpy(preset, &tmp_preset.data, sizeof(PanPreset_t));
}

void flash_init()
{
	eeprom_config_t config;
	EEPROM_GetDefaultConfig(&config);
	EEPROM_Init(EEPROM, &config, BOARD_BOOTCLOCKRUN_CORE_CLOCK);

	i.Init();

	printf("size %d count %d realsize %d\n", preset_size, preset_count, sizeof(PanPreset_t));
}

/*
int main(void)
{
  Store s(MEMSIZE);
  Index i(s);

  preset_t p1;
  p1.header.preset_id = 1;
  strcpy((char*)p1.name, "P1");
  
  preset_t p2;
  p2.header.preset_id = 2;
  strcpy((char*)p2.name, "P2");

  i.SavePreset(&p1);
  i.SavePreset(&p2);
  
  i.SavePreset(&p1);
  i.SavePreset(&p1);
  
  return 0;
}

*/
