using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp1_cv2
{
    internal class ClassArray
    {
        public void Run()
        {
            CompositeBrick[] bricks = new CompositeBrick[2];
            bricks[0] = CreateT(1, 1);
            bricks[1] = CreateT(5, 5);
        }

        public CompositeBrick CreateT(int x, int y)
        {
            CompositeBrick b = new CompositeBrick();
            b.X = x;
            b.Y = y;
            b.bricks = new Brick[4];
            b.bricks[0] = new Brick();
            b.bricks[0].X = 0;
            b.bricks[0].Y = 0;

            b.bricks[1] = new Brick() {X = 1, Y = 0};
            b.bricks[2] = new Brick() { X = 2, Y = 0 };
            b.bricks[3] = new Brick() { X = 1, Y = 1 };

            return b;
        }
    }
}
