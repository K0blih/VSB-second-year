using System;
using System.Collections;
using System.Collections.Generic;

namespace Tutorial10
{
    public class NumberList : IEnumerable<int>
    {
        private List<int> data = new List<int>();

        public void Add(int value)
        {
            data.Add(value);
        }

        public IEnumerator<int> GetEnumerator()
        {
            //return new NumberEnumerator() { Data = data };
            for (int i = 0; i < data.Count; i += 2)
            {
                yield return data[i];
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public IEnumerable<int> Multiply(int n)
        {
            foreach (int i in data)
            {
                yield return i * n;
            }
        }
    }
}
