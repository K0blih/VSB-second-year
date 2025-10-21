using System;
using System.Globalization;

namespace cs1_hw2
{
    internal class Program
    {
        public static Calendar ParseTxt(string txt)
        {
            string[] lines = txt.Split(['\r', '\n'], StringSplitOptions.RemoveEmptyEntries);

            Calendar calendar = new Calendar();

            int i = 0;
            while (i < lines.Length)
            {
                string line = lines[i].Trim();

                if (string.IsNullOrEmpty(line))
                {
                    i++;
                    continue;
                }

                if (line.EndsWith(':'))
                {
                    string type = line.Substring(0, line.Length - 1);
                    string name = lines[++i].Trim();
                    string[] dateParts = lines[++i].Trim().Split(';');
                    DateTime start = DateTime.Parse(dateParts[0]);
                    DateTime end = DateTime.Parse(dateParts[1]);

                    if (type == "MEETING")
                    {
                        Attendee[] attendees = Array.Empty<Attendee>();

                        if (i + 1 < lines.Length && !lines[i + 1].EndsWith(':'))
                        {
                            string[] attendeeNames = lines[++i].Split(';', StringSplitOptions.RemoveEmptyEntries);
                            int attendeeCount = attendeeNames.Length;
                            attendees = new Attendee[attendeeCount];
                            for (int a = 0; a < attendeeCount; a++)
                            {
                                attendees[a] = new Attendee(attendeeNames[a]);
                            }
                        }

                        calendar.Add(new Meeting(name, start, end, attendees));
                    }
                    else if (type == "APPOINTMENT")
                    {
                        calendar.Add(new Appointment(name, start, end));
                    }
                    else if (type == "HOLIDAY")
                    {
                        string location = "Neznámé místo";
                        Attendee[] attendees = Array.Empty<Attendee>();

                        if (i + 1 < lines.Length && !lines[i + 1].EndsWith(':'))
                        {
                            location = lines[++i].Trim();
                        }

                        if (i + 1 < lines.Length && !lines[i + 1].EndsWith(':'))
                        {
                            string[] attendeeNames = lines[++i].Split(';', StringSplitOptions.RemoveEmptyEntries);
                            int attendeeCount = attendeeNames.Length;
                            Array.Resize(ref attendees, attendeeCount);
                            for (int a = 0; a < attendeeCount; a++)
                            {
                                attendees[a] = new Attendee(attendeeNames[a]);
                            }
                        }

                        calendar.Add(new Holiday(name, start, end, attendees, location));
                    }
                }

                i++;
            }

            return calendar;
        }

        static void Main(string[] args)
        {
            Thread.CurrentThread.CurrentCulture = CultureInfo.GetCultureInfo("cs-CZ");
            string txt = File.ReadAllText(args[0]);
            Calendar calendar = ParseTxt(txt);

            Event[] events = calendar[2025, 10, 15];
            Event[] upcomingEvents = calendar.GetAllUpcoming();

            //EVENTS
            Console.WriteLine("DNEŠNÍ UDÁLOSTI:");
            Console.WriteLine("----------");
            foreach (var e in events)
            {
                Console.WriteLine(e.ToString());
            }
            Console.WriteLine();

            //PEOPLE
            Console.WriteLine("LIDÉ SE KTERÝMI SE DNES UVIDÍM:");
            Console.WriteLine("----------");
            foreach (var e in events)
            {
                if (e is Meeting m)
                {
                    for (int i = 0; i < m.Attendees.Length; i++)
                    {
                        Console.WriteLine($"{m.Attendees[i].ToString()} - {e.Start.ToString()}");
                    }
                }
                if (e is Holiday h)
                {
                    for (int i = 0; i < h.Attendees.Length; i++)
                    {
                        Console.WriteLine($"{h.Attendees[i].ToString()} - {e.Start.ToString()}");
                    }
                }
            }
            Console.WriteLine();

            //REMINDER
            Console.WriteLine("ČASY PŘIPOMENUTÍ DNEŠNÍCH UDÁLOSTÍ:");
            Console.WriteLine("----------");
            foreach (var e in events)
            {
                Console.WriteLine($"{e.ToString()} => {e.GetReminderTime()}");
            }
            Console.WriteLine();

            //FUTURE EVENTS
            Console.WriteLine("BUDOUCÍ UDÁLOSTI:");
            Console.WriteLine("----------");
            foreach (var e in upcomingEvents)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
