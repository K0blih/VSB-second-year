using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal class Meeting : Event, IAttendees
    {
        public Attendee[] Attendees { get; set; } = Array.Empty<Attendee>();

        public Meeting(string name, DateTime start, DateTime end, Attendee[] attendees) : base(name, start, end)
        {
            Attendees = attendees;
        }

        public override DateTime? GetReminderTime()
        {
            return Start.AddMinutes(-(30 + 10 * Attendees.Length));
        }
    }
}
