using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Database
{
    public class Person
    {
        public string Name { get; set; }
        private int? age;
        public int? Age {
            get
            {
                return age;
            }
            set
            {
                if (value < 0 || value  > 150) { value = null; }
                this.age = value;
            }
        }
        public GenderEnum? Gender { get; set; }
        public bool IsAdult
        {
            get
            {
                return age >= 18;
            }
        }

        public override string ToString()
        {
            return $"{Name}, Age: {Age}, Gender: {Gender}";
        }
    }
}
