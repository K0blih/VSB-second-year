using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal class Holiday : Event, IAttendees
    {
        public Attendee[] Attendees { get; set; } = Array.Empty<Attendee>();
        private string location = "Neznámé místo";
        public string Location
        {
            get
            {
                return string.IsNullOrWhiteSpace(location) ? "Neznámé místo" : location;
            }
            set
            {
                location = value;
            }
        }

        public Holiday(string name, DateTime start, DateTime end, Attendee[] attendees, string location) : base(name, start, end)
        {
            Attendees = attendees;
            Location = location;
        }

        public override DateTime? GetReminderTime()
        {
            return null;
        }

        public override string ToString()
        {
            return $"{base.ToString()} ({Location})";
        }
    }
}
