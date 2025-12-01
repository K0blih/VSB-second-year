using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tutorial10
{
    internal class NumberEnumerator : IEnumerator<int>
    {
        public List<int> Data { get; set; }
        private int index = -2;
        public int Current
        { 
            get { return Data[index]; }
        }

        object IEnumerator.Current => Current;

        public void Dispose()
        {

        }

        public bool MoveNext()
        {
            index += 2;
            return index < Data.Count;
        }

        public void Reset()
        {
            index = -2;
        }
    }
}
