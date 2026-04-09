using System;
using System.Globalization;
using System.Windows.Data;

namespace cv7
{
    public class UpperConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (value as string)?.ToUpper(culture) ?? string.Empty;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (value as string)?.ToUpper(culture) ?? string.Empty;
        }
    }
}
