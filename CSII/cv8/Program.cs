using System.Net.Http.Json;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace cv8
{
    internal class Program
    {
        static async Task Main(string[] args)
        {
            using HttpClient client = new HttpClient();

            //string img = "https://wildnet.org/wp-content/uploads/learn-about-elephants-program-01.jpg";
            //string url = "https://www.7timer.info/bin/astro.php?lon=18.160005506399536&lat=49.831015379859586&ac=0&unit=metric&output=json&tzshift=0";
            string bin = "https://www.postb.in/1776325170747-1782878206577";
            bin += "?promenna=test&cislo=200";
            //using HttpResponseMessage response = await client.GetAsync(img);

            using HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Post, bin);
            request.Headers.Add("X-MujNazev", "hodnota");
            request.Headers.Add("X-Dalsi", "200");
            request.Headers.Authorization = new System.Net.Http.Headers.AuthenticationHeaderValue("Bearer", "myapikey");
            // **httpContext**.Request.Headers["X-MujNazev"];

            //request.Content = new FormUrlEncodedContent(new Dictionary<string, string>()
            //{
            //    { "promenna", "hodnota" },
            //    { "cislo", "200" }
            //});
            //public IActionResult NameAction(string name, int number)
            //request.Content = new StringContent(
            //    JsonSerializer.Serialize(new
            //    {
            //        model = "gpt-5.4",
            //        input = "Write a short bedtime story about a unicorn."
            //    }),
            //    Encoding.UTF8,
            //    "application/json"
            //    );

            using FileStream fs = new FileStream("img.jpg", FileMode.Open);

            MultipartFormDataContent multipart = new MultipartFormDataContent();
            multipart.Add(new StringContent("Hodnota promenne"), "nazevPromenne");
            multipart.Add(new StringContent("200"), "cislo");
            multipart.Add(new StreamContent(fs), "soubor");
            request.Content = multipart;

            // public IACtionoResult NazevAkce()

            using HttpResponseMessage response = await client.SendAsync(request);

            //using HttpResponseMessage response = await client.GetAsync(bin);
            response.EnsureSuccessStatusCode();

            //if (!response.IsSuccessStatusCode)
            ////if (response.StatusCode != System.Net.HttpStatusCode.OK)
            //{
            //    throw new Exception("Chybna odpoved!");
            //}

            //foreach (var header in response.Headers)
            //{
            //    Console.WriteLine($"{header.Key}: {header.Value.First()}");
            //}

            //using Stream stream = await response.Content.ReadAsStreamAsync();
            //using FileStream fs = new FileStream("img.jpg", FileMode.Create);
            //await stream.CopyToAsync(fs);



            //Weather weather = await response.Content.ReadFromJsonAsync<Weather>(new System.Text.Json.JsonSerializerOptions
            //{
            //    PropertyNameCaseInsensitive = true
            //});

            //foreach (Item item in weather.dataseries)
            //{
            //    Console.WriteLine($"{item.timepoint}: {item.temp2m} °C");
            //}

            //string data = await client.GetStringAsync(url);

            //Console.WriteLine(data.Substring(0, 300));
        }
    }
}
