using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Messages
{
    public class EmailMessage : IMessage
    {
        public string Email { get; set; }
        public string Text { get; set; }
        public EmailMessage() { }
        public EmailMessage(string email, string text)
        {
            Email = email;
            Text = text;
        }

        public void Deserialize(string inputString)
        {
            string[] subStr = inputString.Trim().Split(':');
            Email = subStr[0];
            Text = subStr[1];
        }

        public char GetMark()
        {
            return 'E';
        }

        public string Serialize()
        {
            return $"{Email}:{Text}";
        }
    }
}
