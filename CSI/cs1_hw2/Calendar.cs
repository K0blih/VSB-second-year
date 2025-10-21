using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw2
{
    internal class Calendar
    {
        private Event[] events = new Event[3];
        private int eventCount = 0;

        public void Add(Event ev)
        {
            if (events.Length == eventCount)
            {
                Array.Resize(ref events, eventCount + 5);
            }

            events[eventCount++] = ev;
        }

        public Event[] this[int year, int month, int day]
        {
            get
            {
                DateTime dayStart = new DateTime(year, month, day, 0, 0, 0);
                DateTime dayEnd = dayStart.AddDays(1).AddTicks(-1);
                Event[] result = new Event[3];
                int count = 0;

                for (int i = 0; i < eventCount; i++)
                {
                    if (events[i].Start <= dayEnd && events[i].End >= dayStart)
                    {
                        if (result.Length == count)
                        {
                            Array.Resize(ref result, count + 5);
                        }

                        result[count++] = events[i];
                    }
                }

                Array.Resize(ref result, count);
                return result;
            }
        }

        public Event[] GetAllUpcoming()
        {
            Event[] upcomingEvents = new Event[3];
            int upcomingCount = 0;

            for (int i = 0; i < eventCount; i ++)
            {   
                if (events[i].IsUpcoming)
                {
                    if (upcomingEvents.Length == upcomingCount)
                    {
                        Array.Resize(ref upcomingEvents, upcomingCount + 5);
                    }

                    upcomingEvents[upcomingCount++] = events[i];
                }
            }

            Array.Resize(ref upcomingEvents, upcomingCount);
            return upcomingEvents;
        }
    }
}
