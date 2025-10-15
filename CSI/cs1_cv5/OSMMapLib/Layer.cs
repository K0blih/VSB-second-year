using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OSMMapLib
{
    public class Layer
    {
        public string UrlTemplate { get; private set; }
        public int MaxZoom { get; private set; }

        public Layer(string urlTemplate = "https://{c}.tile.openstreetmap.org/{z}/{x}/{y}.png", int maxZoom = 10)
        {
            UrlTemplate = urlTemplate;
            MaxZoom = maxZoom;
        }

        public string FormatUrl(int x, int y, int zoom)
        {
            Random random = new Random();
            int offset = random.Next(0, 3);
            char c = (char)('a' + offset);

            string tmp = UrlTemplate.Replace("{c}", c.ToString());
            tmp = tmp.Replace("{x}", x.ToString());
            tmp = tmp.Replace("{y}", y.ToString());
            tmp = tmp.Replace("{z}", zoom.ToString());

            return tmp;
        }

        public Tile this[int x, int y, int zoom]
        {
            get
            {
                return new Tile(x, y, zoom, FormatUrl(x, y, zoom));    
            }
        }
    }
}
