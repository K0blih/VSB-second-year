using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal abstract class Event
    {
        private DateTime start;
        private DateTime end;
        public string Name { get; set; }
        public DateTime Start
        {
            get { return start; }
            set { start = value; }
        }
        public DateTime End
        {
            get { return end; }
            set
            {
                end = value;
                if (end < start)
                {
                    (start, end) = (end, start);
                }
            }
        }
        public bool IsUpcoming
        {
            get
            {
                return start > new DateTime(2025, 10, 15, 15, 0, 0);
            }
        }

        public Event(string name, DateTime start, DateTime end)
        {
            Name = name;
            Start = start;
            End = end;
        }

        public abstract DateTime? GetReminderTime();

        public override string ToString()
        {
            return $"{Name}: {Start} - {End}";
        }
    }
}
