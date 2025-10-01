using System.Globalization;

namespace cs1_cv3
{
    enum ParseIntOption
    {
        NONE = 1,
        ALLOW_WHITESPACES = 2,
        ALLOW_NEGATIVE_NUMBERS = 4,
        IGNORE_INVALID_CHARACTERS = 8
    }
    internal class Program
    {
        static int ParseInt(string s)
        {
            int number = 0;
            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];
                int n = c - '0'; //48

                if (n < 0 || n > 9)
                {
                    return -1;
                }

                number = number * 10 + n;
            }

            return number;
        }

        static int? ParseIntOrNull(string s)
        {
            int? number = 0;
            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];
                int n = c - '0'; //48

                if (n < 0 || n > 9)
                {
                    return null;
                }

                number = number * 10 + n;
            }

            return number;
        }

        //static bool TryParseInt(string s, out int number)
        //{
        //    number = 0;
        //    for (int i = 0; i < s.Length; i++)
        //    {
        //        char c = s[i];
        //        int n = c - '0'; //48

        //        if (n < 0 || n > 9)
        //        {
        //            return false;
        //        }

        //        number = number * 10 + n;
        //    }

        //    return true;
        //}

        static bool TryParseInt(string s, out int number, ParseIntOption options)
        {
            number = 0;
            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];
                int n = c - '0'; //48

                if (n < 0 || n > 9)
                {
                    if (options.HasFlag(ParseIntOption.ALLOW_WHITESPACES) && c == ' ') { continue; }
                    if (options.HasFlag(ParseIntOption.IGNORE_INVALID_CHARACTERS)) { continue; }
                    if (options.HasFlag(ParseIntOption.ALLOW_NEGATIVE_NUMBERS)) { continue; } //TODO
                    return false;
                }

                number = number * 10 + n;
            }

            return true;
        }

        static void Main(string[] args)
        {
            Console.Write("Input number: ");
            string input = Console.ReadLine();

            //int number = ParseInt(input);
            //int? number = ParseIntOrNull(input);

            //Console.WriteLine(number);

            if (TryParseInt(input, out int number))
            {
                Console.WriteLine(number);
            }
            else
            {
                Console.WriteLine("Isn't a number.");
            }

            //CultureInfo ciCZ = CultureInfo.GetCultureInfo("cs-CZ");
            //CultureInfo ciUS = CultureInfo.GetCultureInfo("en-US");
            //CultureInfo ciUK = CultureInfo.GetCultureInfo("en-UK");

            //double number = double.Parse(input, ciCZ);

            //Console.WriteLine(number.ToString(ciUS));


        }
    }
}
