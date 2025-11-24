using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv9
{
    internal class ContactComparer : IComparer<Contact>
    {
        public int Compare(Contact? x, Contact? y)
        {
            return x.Name.CompareTo(y.Name) * -1;
        }
    }
}
