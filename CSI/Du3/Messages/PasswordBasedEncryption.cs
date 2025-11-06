using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class PasswordBasedEncryption : IEncryption
    {
        private readonly string password = "";
        public PasswordBasedEncryption(string inputString)
        {
            if (string.IsNullOrEmpty(inputString))
            {
                throw new InvalidPasswordException();
            }
            password = inputString;
        }

        public string Decrypt(string inputString)
        {
            StringBuilder sb = new StringBuilder(inputString.Length);

            for (int i = 0; i < inputString.Length; i++)
            {
                char encChar = inputString[i];
                char passChar = password[i % password.Length];
                int diff = encChar - passChar;

                if (diff < 0)
                {
                    throw new InvalidPasswordException(
                        "Negative character code difference during decryption.",
                        inputString,
                        password
                    );
                }

                sb.Append((char)diff);
            }

            return sb.ToString();
        }

        public string Encrypt(string inputString)
        {
            StringBuilder sb = new StringBuilder(inputString.Length);

            for (int i = 0; i < inputString.Length; i++)
            {
                char inputChar = inputString[i];
                char passChar = password[i % password.Length];
                int sum = inputChar + passChar;
                sb.Append((char)sum);
            }

            return sb.ToString();
        }

        public char GetMark()
        {
            return 'P';
        }
    }
}
