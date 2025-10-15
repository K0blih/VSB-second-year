using OSMMapLib;

namespace OSMMap
{
    internal class Program
    {
        class CycleLayer : Layer
        {
            public CycleLayer() : base("https://b.tile-cyclosm.openstreetmap.fr/cyclosm/{z}/{x}/{y}.png" , 17) { }
        }

        static void Main(string[] args)
        {
            Tile tile = new Tile(2, 2, 2, "url");
            Console.WriteLine(tile);

            Layer layer = new Layer(maxZoom: 17);
            Console.WriteLine(layer);

            Tile t = layer[0, 2, 5];
            Console.WriteLine(t);

            Map map = new Map();
            //map.Layer = layer;
            map.Layer = new CycleLayer();
            map.Zoom = 17;
            map.Lat = 49.832219198398164;
            map.Lon = 18.161564793114902;
            map.Render("output.png");
        }
    }
}
