using System.Text;

namespace Tutorial10
{
    public class Translator
    {
        public void Add(string source, string target)
        {
            // TODO: doplnit...
        }


        public string Translate(string txt)
        {
            string[] words = txt.Split(' ');

            StringBuilder result = new StringBuilder();

            foreach (string word in words)
            {
                // TODO: doplnit...

            }

            return result.ToString();
        }



    }
}
