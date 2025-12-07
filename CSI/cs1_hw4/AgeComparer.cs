using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw4
{
    internal class AgeComparer : IComparer<Customer>
    {
        public int Compare(Customer? x, Customer? y)
        {
            return x!.Age.CompareTo(y!.Age);
        }
    }
}
