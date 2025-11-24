using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv9
{
    internal class NumberComparer : IComparer<int>
    {
        public int Compare(int x, int y)
        {
            if (x == y) { return 0; }

            if (x % 2 == 0 && y % 2 == 0)
            {

                return x < y ? -1 : 1;
            }

            if (x % 2 == 1 && y % 2 == 1)
            {

                return x < y ? -1 : 1;
            }

            return x % 2 == 0 ? 1 : -1;
        }
    }
}
