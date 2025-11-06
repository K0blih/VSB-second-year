using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class InvalidPasswordException : Exception
    {
        public string EncryptedText { get; }
        public string Password { get; }
        public InvalidPasswordException() { }
        public InvalidPasswordException(string message, string encryptedText, string password) : base(message) 
        {
            EncryptedText = encryptedText;
            Password = password;
        }
    }
}
