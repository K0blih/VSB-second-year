using System;
using System.Collections.Generic;
using Tutorial10.JsonSerializerExample;
using Tutorial10.XmlSerializerExample;

namespace Tutorial10
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // serializace
            // ----------------------------------------------------------
            //JsonSerialization.Run();
            //XmlSerialization.Run();



           
            
            // enumerátor
            // ---------------------------------------------------------- 
            //NumberList list = new NumberList();
            //for (int i = 0; i < 20; i++)
            //{
            //    list.Add(i);
            //}

            //int count = 0;
            //foreach (int i in list.Multiply(5))
            //{
            //    if (count == 5) { break; }

            //    Console.WriteLine(i);
            //    count++;
            //}

            //using IEnumerator<int> enu = list.GetEnumerator();
            //while (enu.MoveNext())
            //{
            //    Console.WriteLine(enu.Current);
            //}









            
            // kolekce
            // ----------------------------------------------------------

            int frequency = DictionaryStatistics.WordFrequency(@"Jaguár americký (Panthera onca) je kočkovitá šelma patřící do rodu Panthera. Je jediným zástupcem tohoto rodu v Americe. Po tygrovi (P. tigris) a lvu (P. leo) je největší kočkovitou šelmou a na západní polokouli úplně největší. Vyskytuje se od Mexika přes velkou část Střední Ameriky jižně do Paraguaye a severní Argentiny. Několik jedinců žije nedaleko Tucsonu v americkém státě Arizona.

Stavbou těla a zbarvením se nejvíce podobá levhartovi (P. pardus), i když je robustnější a obvykle většího vzrůstu. Jeho chování a areál jsou spíše podobné chování a areálu tygra. Preferuje deštné lesy, ale vyskytuje se i v zalesněných a otevřených terénech. Žije poblíž vody a stejně jako tygr je zdatným plavcem. Je to samotářský predátor lovící různé druhy živočichů. V potravním řetězci je na vrcholu (v hierarchii nemá vyššího predátora) a má velký význam pro přírodní rovnováhu. Jeho skus je mimořádně silný, a to i v poměru k ostatním velkým kočkám (lev, levhart, tygr).[3] To mu umožňuje lovit krunýřem opatřené plazy[4] a používat neobvyklou metodu usmrcování – prokousnutí lebky oběti mezi ušima a usmrcení narušením mozku.[5]

Podle klasifikace IUCN patří mezi téměř ohrožené druhy a jeho populace klesá například z důvodu ztráty přirozeného prostředí. Mezinárodní obchod s jaguáry je zakázaný, ale každoročně zemře při konfliktech s lidmi, hlavně s farmáři v Jižní Americe, mnoho jedinců. Jaguár byl důležitým symbolem mnoha původních amerických kultur, například Mayů a Aztéků.");

            Console.WriteLine(frequency);
            
        }
    }
}
