using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal class Attendee
    {
        public string Name { get; init; }

        public Attendee(string name)
        {
            Name = name;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
