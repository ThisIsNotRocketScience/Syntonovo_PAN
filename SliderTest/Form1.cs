using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SliderTest
{
    public partial class Form1 : Form
    {

        float mousex = 0;
        float mousey = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            var G = e.Graphics;
            G.Clear(Color.Black);
            G.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.High;
            G.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;


            Pen P = new Pen(Color.FromArgb(200, 200, 200), 3);


            Brush B = new SolidBrush(Color.FromArgb(0x00,0x74,0x7f));
            Brush B2 = new SolidBrush(Color.FromArgb(0xff, 0xff-0x74, 0xff-0x7f));
            Brush B3 = new SolidBrush(Color.Black);
            float Margin = 10;
            float PillW = 50;
            float PillH = pictureBox1.Height - Margin * 2;

            DrawPill(G, P,B2,B, Margin, Margin, PillW, PillH, mousey);
            
        }

        void DrawPill(Graphics G, Pen P, Brush B,Brush B2, float x, float y, float width, float  height, float fill)
        {
            float H = height - width ;

            float radius = width / 2.0f;
            if (fill > 0)
            {
                var FPath = new System.Drawing.Drawing2D.GraphicsPath();
                FPath.AddArc(x, y, width, width, 180, 180);

                FPath.AddLine(x + width, y + radius, x + width, y + radius + (height - width) * fill);
                if (fill >= 0.9)
                {
                    FPath.AddArc(x, y + H, width, width, 0, 180);
                }
                FPath.AddLine(x, y + radius + (height - width) * fill, x, y + radius);

                //G.FillPath(B, FPath);
                //x -= 50.0f;
                G.FillPath(B, FPath);

            }
            else
            {
                fill = 1+fill;
                var FPath = new System.Drawing.Drawing2D.GraphicsPath();
                if (fill <= 0.1)
                {
                    FPath.AddArc(x, y, width, width, 180, 180);
                }
                FPath.AddLine(x + width, y + radius + (height - width) * fill, x + width, y + radius + (height - width) * fill);
                    FPath.AddArc(x, y + H, width, width, 0, 180);
                FPath.AddLine(x, y + radius + (height - width) , x, y + radius + (height - width) * fill);
                FPath.Reset();

                //x += 50;
                FPath.AddArc(x, y + (fill *0.5f ) * (height - width), width, width, 180, 180);
                FPath.AddLine(x + width, y + (fill*0.5f+0.5f) * (height - width), x + width, y + radius + (height - width) * 0.5f);
                FPath.AddLine(x, y + radius + 0.5f * (height - width), x, y + radius + (height - width) * (fill*0.5f+0.5f));


                G.FillPath(B2, FPath);
            }
            var Path = new System.Drawing.Drawing2D.GraphicsPath();
            Path.AddArc( x, y, width, width, 180, 180);

            Path.AddLine( x + width, y + radius, x + width, y + height - radius);
            Path.AddArc( x, y + H, width, width, 0, 180);
            Path.AddLine( x, y + height - radius,x, y + radius);

            G.DrawPath(P, Path);

        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            mousex = e.X / (float)pictureBox1.Width;
            mousey = (2.0f * e.Y) / ((float)pictureBox1.Height) - 1.0f; 
            pictureBox1.Invalidate();
        }

        private void pictureBox1_Resize(object sender, EventArgs e)
        {
            pictureBox1.Invalidate();
        }
    }
}
