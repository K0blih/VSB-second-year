using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp1_cv2
{
    internal class MultidimensionalArray
    {
        public void Run()
        {
            bool[,] stage = new bool [10, 20];

            CreateT(stage, 0, 0);

            while(true)
            {
                Console.BackgroundColor = ConsoleColor.Black;
                Console.Clear();
                MainLoop(stage);
                Thread.Sleep(1000);
                Shift(stage);
            }
        }

        public void MainLoop(bool[,] stage)
        {
            Console.BackgroundColor = ConsoleColor.Gray;
            Console.ForegroundColor = ConsoleColor.Red;
            for (int r = 0; r < stage.GetLength(1) ; r++)
            {
                for(int c = 0; c < stage.GetLength(0) ; c++)
                {
                    if (stage[c, r])
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

        public void Shift(bool[,] stage)
        {
            for (int r = stage.GetLength(1) - 1; r > 0; r--)
            {
                for (int c = 0; c < stage.GetLength(0); c++)
                {
                    stage[c, r] = stage[c, r - 1];
                }
            }

            for (int c = 0; c < stage.GetLength(0); c++)
            {
                stage[c, 0] = false;
            }
        }

        public void CreateT(bool[,] stage, int x, int y)
        {
            stage[x, y] = true;
            stage[x + 1, y] = true;
            stage[x + 2, y] = true;
            stage[x + 1, y + 1] = true;
        }
    }
}
