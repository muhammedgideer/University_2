import kotlin.random.Random


fun main() 
{	//dizi tanımı yapılır
    val dizi=Array<Int>(10){0}
    
    for (i in 0..9)
    
    {	//diziye Random atanır
        dizi[i] = Random.nextInt(1, 100)
    }
    //her seferinde yeni dizi yazdırır
    
    print("DIZI : ")
    //Random atanan diziyi olduğu gibi yazdırır
    for(i in 0..9)
    {
     	print(" " + dizi[i])   
    }
    
    
     

    //Bubble Sort
    for (i in 0 until 9) 
    {
        for (j in 0 until 9) 
        {
            if (dizi[j]>dizi[j+1]) 
            {
                val temp:Int = dizi[j]
                dizi[j]=dizi[j+1]
                dizi[j+1]=temp
            }
        }
    }
    
print("\n\nSIRALI : ")

//Sıralanan dizileri yazdırır
for(i in 0..9)
    {
     	print(" " + dizi[i])   
    }

}
