using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal class Appointment : Event
    {
        public Appointment(string name, DateTime start, DateTime end) : base(name, start, end)
        {
        }

        public override DateTime? GetReminderTime()
        {
            return Start.AddMinutes(-30);
        }
    }
}
