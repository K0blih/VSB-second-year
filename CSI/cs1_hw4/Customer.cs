using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw4
{
    public class Customer
    {
        public required string Name { get; set; }
        public int Age { get; set; }

        public override string ToString()
        {
            return $"{Name} ({Age})";
        }
    }
}
