using MailKit.Net.Smtp;
using MimeKit;
using System.Threading.Tasks;

internal class Program
{
    private static async Task Main(string[] args)
    {
        MimeMessage msg = new MimeMessage();
        msg.Subject = "Moje prvni zprava";
        msg.To.Add(new MailboxAddress("jmeno adresata", "...@....cz"));
        msg.From.Add(new MailboxAddress("cviceni", "atnet2019@seznam.cz"));

        BodyBuilder bb = new BodyBuilder();
        bb.TextBody = "Ahoj, Jak se mas?";
        //bb.HtmlBody
        msg.Body = bb.ToMessageBody();

        //

        using SmtpClient smtp = new SmtpClient();
        await smtp.ConnectAsync("smtp.seznam.cz", 465);
        await smtp.AuthenticateAsync("atnet2019@seznam.cz", "cviceni-c#");

        await smtp.SendAsync(msg);

        await smtp.DisconnectAsync(true);
}