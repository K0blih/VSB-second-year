using System.Text.RegularExpressions;
using System.Xml;

namespace cv9
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            //Regex regex = new Regex("^[A-Z]{3}[0-9]{3,4}$", RegexOptions.IgnoreCase | RegexOptions.Compiled);

            //while (true)
            //{
            //    string login = Console.ReadLine();
            //    if (regex.IsMatch(login))
            //    {
            //        Console.WriteLine("Valid");
            //    }
            //    else
            //    {
            //        Console.WriteLine("Not valid");
            //    }
            //}

            //Regex emailRegex = new Regex(@"^[a-zA-Z0-9\.\-]+@[a-z]+\.[a-z]{2,6}$");


            //string tmp = "https://csharp.janjanousek.cz/cv9/";
            //string[] urls = tmp.Split('\n', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

            //Regex urlRegex = new Regex(@"^(https?):\/\/(?:[a-z]+\.)?([a-z]+\.[a-z]{2,6})(?:\/|\?|$)");

            //foreach (string url in urls)
            //{
            //    Console.WriteLine(url);

            //    Match match = urlRegex.Match(url);
            //    if (match.Success)
            //    {
            //        string protocol = match.Groups[1].Value;
            //        string subDomain = match.Groups[2].Value;
            //        string domain = match.Groups[3].Value;
            //        Console.WriteLine(protocol);
            //        Console.WriteLine(subDomain);
            //        Console.WriteLine(domain);
            //    }
            //    else
            //    {
            //        Console.WriteLine("Chybny format dat.");
            //    }

            //    Console.WriteLine();
            //}

            //Dictionary<string, string> data = new Dictionary<string, string>()
            //{
            //    { ... },
            //    { ... },
            //    { ... }
            //};

            //string txt = "";

            //Regex regex = new Regex("");

            //string result = regex.Replace(txt, (Match match) =>
            //{
            //    string key = match.Groups[1].Value;
            //    return data.ContainsKey(key) ? data[key] ? data[key] : "???";
            //});

            //Console.WriteLine(result);

            using HttpClient client = new HttpClient();
            using HttpResponseMessage response = await client.GetAsync("https://www.lupa.cz/rss/clanky/");
            response.EnsureSuccessStatusCode();
            string xml = await response.Content.ReadAsStringAsync();

            XmlDocument xDoc = new XmlDocument();
            xDoc.LoadXml(xml);

            //foreach (XmlNode titleTextNode in xDoc.SelectNodes("/rss/channel/item/title/text()"))
            foreach (XmlNode titleTextNode in xDoc.SelectNodes("//item"))
            {
                string title = titleTextNode.SelectSingleNode("title/text()").Value;
                string link = titleTextNode.SelectSingleNode("link/text()").Value;
                Console.WriteLine(title);
                Console.WriteLine(link);
                Console.WriteLine();
            }

            xDoc.Save("data.xml");

            //XmlNode rootNode = xDoc.CreateElement("root");
            //xDoc.AppendChild(rootNode);

            //for (int i = 0; i < 5; i++)
            //{
            //    XmlNode customerNode = xDoc.CreateElement("customer");
            //    rootNode.AppendChild(customerNode);

            //    XmlNode nameNode = xDoc.CreateElement("name");
            //    customerNode.AppendChild(nameNode);

            //    nameNode.AppendChild(xDoc.CreateTextNode("Jan " + i));

            //    XmlAttribute idAttr = xDoc.CreateAttribute("id");
            //    idAttr.Value = i.ToString();
            //    customerNode.Attributes.Append(idAttr);
            //}

            //rootNode.RemoveChild(rootNode.ChildNodes[1]);

            //xDoc.Save("data.xml");
        }
    }
}
