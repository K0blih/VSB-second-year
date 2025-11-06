using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public interface IEncryption
    {
        string Encrypt(string inputString);
        string Decrypt(string inputString);
        char GetMark();
    }
}
