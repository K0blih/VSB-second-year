using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv7
{
    internal interface IKeyValuePair<T, U>
    {
        T Key { get; }
        U Value { get; }
    }
}
