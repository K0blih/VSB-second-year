using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv7
{
    internal class ArrayHelper<T>
    {
        public static void Swap(T[] arr, int idx1, int idx2)
        {
            //(arr[idx2], arr[idx1]) = (arr[idx1], arr[idx2]);
            T tmp = arr[idx1];
            arr[idx1] = arr[idx2];
            arr[idx2] = tmp;
        }

        public static T[] Concat(T[] arr1,  T[] arr2)
        {
            int newLength = arr1.Length + arr2.Length;
            int index = 0;
            T[] concatArray = new T[newLength];

            //Array.Copy(arr1, concatArray, arr1.Length);
            //Array.Copy(arr2, 0, concatArray, arr1.Length, arr2.Length);

            for (int i = 0; i < arr1.Length; i++)
            {
                concatArray[index++] = arr1[i];
            }

            for (int i = 0; i < arr2.Length; i++)
            {
                concatArray[index++] = arr2[i];
            }

            return concatArray;
        }
    }
}
