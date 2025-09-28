using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp1_cv2
{
    internal class JaggedArray
    {
        public void Run()
        {
            bool[][] stage = new bool[20][];
            for (int i = 0; i < stage.Length; i++)
            {
                stage[i] = new bool[10];
            }

            CreateT(stage, 0, 0);

            while (true)
            {
                Console.BackgroundColor = ConsoleColor.Black;
                Console.Clear();
                MainLoop(stage);
                Thread.Sleep(1000);
                Shift(stage);
            }
        }

        public void MainLoop(bool[][] stage)
        {
            Console.BackgroundColor = ConsoleColor.Gray;
            Console.ForegroundColor = ConsoleColor.Red;
            for (int r = 0; r < stage.Length; r++)
            {
                for (int c = 0; c < stage[r].Length; c++)
                {
                    if (stage[r][c])
                    {
                        Console.Write("\u2588\u2588");
                    }
                    else
                    {
                        Console.Write("  ");
                    }
                }
                Console.WriteLine();
            }
        }

        public void Shift(bool[][] stage)
        {
            for (int r = stage.Length - 1; r > 0; r--)
            {
                stage[r] = stage[r - 1];
            }

            for (int c = 0; c < stage[0].Length; c++)
            {
                stage[0] = new bool[stage[0].Length];
            }
        }

        public void CreateT(bool[][] stage, int x, int y)
        {
            stage[y][x] = true;
            stage[y][x + 1] = true;
            stage[y][x + 2] = true;
            stage[y + 1][x + 1] = true;
        }
    }
}
