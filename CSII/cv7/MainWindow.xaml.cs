using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace cv7
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public ObservableCollection<Customer> Customers { get; set; }

        public MainWindow()
        {
            this.Customers = [new Customer() { Id = 1, FirstName = "Jan", LastName = "Novak", Age = 30 }];

            InitializeComponent();

            this.DataContext = this;

            //Button btn = new Button() { Content = "klikni na me!" };
            //btn.Click += (object sender, RoutedEventArgs args) =>
            //{
            //    btn.Content = "Kliknuto";
            //};
            //this.Content = btn;
        }

        private void AddCustomer(object sender, RoutedEventArgs e)
        {
            this.Customers.Add(new Customer() { Id = 2, FirstName = "Jan", LastName = "Novak", Age = 40 });
        }

        private void RemoveCustomer(object sender, RoutedEventArgs e)
        {
            Button btn = (Button)sender;
            Customer customer = (Customer)btn.DataContext;
            this.Customers.Remove(customer);
        }

        private void AnonymizeCustomer(object sender, RoutedEventArgs e)
        {
            Button btn = (Button)sender;
            Customer customer = (Customer)btn.DataContext;
            customer.FirstName = "***";
            customer.LastName = "***";
        }

        private void EditCustomer(object sender, RoutedEventArgs e)
        {
            Button btn = (Button)sender;
            Customer customer = (Customer)btn.DataContext;

            EditDialog dialog = new EditDialog(customer);
            dialog.ShowDialog();
        }
    }
}