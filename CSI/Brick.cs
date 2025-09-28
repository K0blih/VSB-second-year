using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Csharp1_cv2
{
    internal class Brick
    {
        public int X;
        public int Y;
    }

    internal class CompositeBrick
    {
        public Brick[] bricks;
        public int X;
        public int Y;
    }
}
