using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace cv7
{
    public class Customer : INotifyPropertyChanged
    {
        public int Id { get; set; }

        private string firstname = string.Empty;
        public string FirstName
        {   
            get { return firstname; }
            set
            {
                SetProp(ref firstname, value);

                //firstname = value;
                //PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(FirstName)));
            }
        }

        private string lastName = string.Empty;
        public string LastName { get { return lastName; } set { SetProp(ref lastName, value); } }
        public int Age { get; set; }

        public event PropertyChangedEventHandler? PropertyChanged;

        private void SetProp(ref string prop, string value, [CallerMemberName] string? name = null)
        {
            prop = value;
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
        }
    }
}
