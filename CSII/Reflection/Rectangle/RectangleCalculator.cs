namespace Rectangle
{
	public class RectangleCalculator
	{
		public double Width { get; set; }
		public double Height { get; set; }

		public double GetArea()
		{
			return Width * Height;
		}

		public string GetArea(string units)
		{
			return GetArea() + units;
		}

	}
}
