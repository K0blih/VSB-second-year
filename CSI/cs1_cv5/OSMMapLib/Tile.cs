namespace OSMMapLib
{
    public class Tile
    {
        public int X { get; private set; }
        public int Y { get; private set; }
        public string Url { get; private set; }

        private int zoom;
        public int Zoom
        {
            get
            {
                return zoom;
            }
            private set
            {
                if (value < 1)
                {
                    value = 1;
                }
                zoom = value;
            }
        }

        public Tile(int x, int y, int zoom, string url)
        {
            X = x;
            Y = y;
            Url = url;
            Zoom = zoom;
        }

        public override string ToString()
        {
            return $"[{X}, {Y}, {Zoom}]: {Url}";
        }
    }
}
