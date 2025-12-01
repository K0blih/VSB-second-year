using System;
using System.Collections.Generic;
using System.Text;

namespace Tutorial10
{
    public class DictionaryStatistics
    {
        public static int WordFrequency(string text)
        {
            string[] words = text.Split(
                [' ', '.', '(', ')'],
                System.StringSplitOptions.RemoveEmptyEntries | System.StringSplitOptions.TrimEntries
            );

            Dictionary<string, int> dict = new Dictionary<string, int>();
            //SortedDictionary<string, int> dict = new SortedDictionary<string, int>();

            foreach (string word in words)
            {
                if (dict.ContainsKey(word))
                {
                    dict[word] = dict[word] + 1;
                }
                else
                {
                    dict[word] = 1;
                }
            }

            int count = 0;
            string topWord = null;
            foreach (KeyValuePair<string, int> pair in dict)
            {
                Console.WriteLine(pair.Key + ": " + pair.Value);

                if (pair.Value > count)
                {
                    count = pair.Value;
                    topWord = pair.Key;
                }
            }

            Console.WriteLine();
            Console.WriteLine("Nejcastejsi slovo: " + topWord);
            return count;
        }



    }
}
