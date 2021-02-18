#include <stdio.h> 
#include <stdlib.h>
#include <limits.h> 
#include <string.h>


static char indexarray[200000][50]; // Her bir oyuncunun eklendiði dizi. Oyuncunun bu dizideki indexi grapha eklenir.
static int nextindex = 0; // Yukarýdaki diziye oyuncu eklerken sýrayý tutan deðiþken.
static char indexmov[30000][100]; // Her bir filmin eklendiði dizi. Dizilerin indexi graphlardaki düðümlerde tutulacak.
static int nextmov = 0; // Yukarýdaki diziye film eklerken sýrayý tutan deðiþken.
static int visited[200000][3]; 
// BFS algoritmasý kullanýlýrken bir düðüm ziyaret edildi mi bilgisini tutan, ayrýca o düðümün Kevin Bacona uzaklýðýný tutan dizi.
// Bu diziyi tek boyuta indirgemeyi denedim ama küçük bir kaç sebepten ötürü bu þekilde yapmak daha kolay oldu.
// Ayrýca graph içinde de tutulabilirdi. Static olarak tanýmlamak daha kolay olacaðý için bu þekilde yaptým.
static int path[200000]; // bir düðüme hangi düðümden gelindiðini tutan dizi.
static int baconsum = 0; 
// Kaç hangi uzaklýkta kaç adet düðüm varsa hepsinin toplamýný tutan deðiþken.
// nextindex - baconsum yaparak kaç adet düðümün sonsuz uzaklýkta olduðu elde edilebiliyor.

struct Queue // Kuruk veri yapýsý. ön arka ve dizinin kapasitesi tutuluyor.
{ 
    int front, rear, size; 
    int capacity; 
    int* array; 
}; 

struct Queue* createQueue (int capacity) 
// Bir kapasite verilerek kuyruk oluþturmaya yarayan fonksiyon.
{ 
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
    queue->capacity = capacity; 
    queue->front = queue->size = 0;  
    queue->rear = capacity - 1; 
    queue->array = (int*) malloc(queue->capacity * sizeof(int)); 
    return queue; 
} 

int dolumu (struct Queue* queue) 
{
// Kuyruðun dolu olup olmadýðýný döner.
	return (queue->size == queue->capacity);  
} 
  
int bosmu (struct Queue* queue)
// Kuyruðun boþ olup olmadýðýný döner.
{
	return (queue->size == 0);
} 

void elemanekle (struct Queue* queue, int item)
// Kuyruða eleman ekleyen fonksiyon.
{ 
    if (dolumu(queue)) 
        return; 
    queue->rear = (queue->rear + 1) % queue->capacity; 
    queue->array[queue->rear] = item; 
    queue->size = queue->size + 1; 
} 

int elemansil (struct Queue* queue)
// Kuyruktan eleman silen fonksiyon. 
{ 
    if (bosmu(queue)) 
        return INT_MIN; 
    int item = queue->array[queue->front]; 
    queue->front = (queue->front + 1) % queue->capacity; 
    queue->size = queue->size - 1; 
    return item; 
} 


struct node
// Grapha eklenecek oyuncular. dest oyuncuya özel sayý deðeri.
// movie,  2  oyuncunun hangi filmle birbirlerine baðlý olduðunu tutan deðiþken.
{ 
	int dest;
	int movie;
	struct node* next;
}; 

struct komsuliste 
// Graphý oluþturan listeler.
{ 
	struct node *head;
}; 

struct Graph 
// V adet listeden oluþan graph. array bu listelerin dizisi.
{ 
	int V; 
	struct komsuliste* array;
}; 

struct node* yeninode (int dest) 
// oyuncuya ait düðüm oluþturan fonksiyon.
{ 
	struct node* yeni = (struct node*) malloc(sizeof(struct node)); 
	yeni->dest = dest; 
	yeni->next = NULL;
	return yeni; 
} 

struct Graph* createGraph (int V) 
// Parametre olarak verilen V deðeri büyüklüðünde Graph oluþturan fonksiyon.
{ 
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph)); 
	graph->V = V;

	graph->array = (struct komsuliste*) malloc(V * sizeof(struct komsuliste)); 

	int i; 
	for (i = 0; i < V; ++i)
	{
		graph->array[i].head = NULL; 
	} 
	return graph; 
}

void graphekle (struct Graph* graph, int src, int dest, int mov) 
// deðerleri verilen 2 oyuncuyu birbirine verilen film ile baðlayan fonksiyon.
// oyuncu düðümlerinde tutulan movie deðerini mov parametresinden alýyor.
{ 	
	struct node* yeni = yeninode(dest);
	yeni->next = graph->array[src].head;
	yeni->movie = mov;
	graph->array[src].head = yeni;

	yeni = yeninode(src); 
	yeni->next = graph->array[dest].head;
	yeni->movie = mov;
	graph->array[dest].head = yeni; 
} 

int addtoarray (char *name)
// ismi verilen oyuncuyu static olarak tanýmlanmýþ diziye ekleyen ve dizideki indexini döndüren fonksiyon.
// oyuncu zaten dizideyse direkt olarakdizideki yerini döndürür.
{
	int i;
	int j;
	
	for(i = 0; i < nextindex; i++)
	{
		if(strcmp(indexarray[i], name) == 0)
		{
			return i;
		}
	}
	strcpy(indexarray[nextindex], name);
	nextindex++;
	return nextindex - 1;
}

int addtomov (char *name)
// addtoarray fonksiyonuyla ayný iþi filmler için yapan fonksiyon.
{
	int i;
	int j;
	
	for(i=0;i<nextmov;i++)
	{
		if(strcmp(indexmov[i], name)==0)
		{
			return i;
		}
	}
	
	strcpy(indexmov[nextmov], name);
	nextmov++;
	return nextmov - 1;
}

void bfs (struct Graph* graph, int start)
// BFS algoritmasýný uygulayan fonksiyon.
{
	struct Queue* queue = createQueue(nextindex); 
	
	visited[start][0] = 1;
	visited[start][1] = 0;
	elemanekle(queue, start);
	path[start] = -1;
	
	while(bosmu(queue) == 0)
	{
		int curr;
		curr = elemansil(queue);
		struct node* temp;
		temp = graph->array[curr].head;
			
		while(temp != NULL)
		{	
			int g;
			g = temp->dest;
					
			if(visited[g][0] == 0)
			{
				visited[g][0] = 1;
				visited[g][1] = visited[curr][1]+1;
				elemanekle(queue, g);
				path[g] = curr;
			}
			temp = temp->next;	
		}		
	}
}

void kevinbaconsayisi (struct Graph* graph, char *artist)
// Ýsmi verilen kiþinin kevin bacona kaç film uzakta olduðunu bulan fonksiyon.
// Ayrýca bu kiþinin Kevin bacona nasýl ulaþtýðýný da yazdýrýr.
{
	int b;
	int a;
	int j = 0;
	b = addtoarray(artist);
		
	struct node* temp1;
	a = path[b];
	
	printf("\n");
	if (visited[b][0] == 0)
	{
		printf("\n\n%s's Kevin Bacon Number is: Infinite", artist);
		return;
	}
	while(a != -1)
	{
		temp1 = graph->array[a].head;
		
		while(temp1 != NULL)
		{
			if(temp1->dest == b)
			{
				printf("%s - %s :  %s\n", indexarray[b], indexarray[a], indexmov[temp1->movie]);
				j++;
			}		
			temp1 = temp1->next;
		}
		b = a;
		a = path[a];
	}
	printf("\n\n%s's Kevin Bacon Number is: %d", artist, j);
}

void herbirbaconsayisi()
// verilen dosyada hangi kevin bacon sayýsýnda kaç kiþi olduðunu bulan ve yazan fonksiyon.
{
	printf("\n\n\n");
	printf(" 0 Kevin Bacon sayisina sahip kisi sayisi: 1 \n");
	int sum, j, i;
	for(j = 1; j < nextindex; j++)
	{	
		sum = 0;
		
		for(i = 0; i < nextindex; i++)
		{
			if(visited[i][1] == j)
			{
				sum++;
			}
		}
		if(sum == 0)
		{
			break;
		}
		baconsum += sum;
		printf(" %d Kevin Bacon sayisina sahip kisi sayisi: %d \n", j, sum);		
	}
	printf(" Kevin Bacon sayisi sonsuz olan kisi sayisi: %d", nextindex - baconsum);
}

int main() 
{  
	int V = 200000; // Graphýn büyüklüðü input-3 dosyasýna uygun olarak 200k atanýr.
	int n = 10000; // bir satýrda 10000 chara kadar okuma yapmak için. 10000 olmasýnýn özel bir sebebi yok. Yüksek olmasý önemli.
	int i = 0; // iterators
	int j = 0;
	int k = 0;
	struct Graph* graph = createGraph(V); // graph isimli graph oluþturuldu.
	
	FILE *fp;
	char dosya[20];
	printf("Name Of  The File (.txt): "); 
	scanf ("%s", dosya);
	fp = fopen(dosya, "r");
	// dosyanýn ismi kullanýcýdan alýndý ve read modunda açýldý.
	
	char line[10000]; // tüm satýrýn tutulacaðý string
	int m;
	int x;
	int y;
	char delim[] = "/"; // strtok fonksiyonu kullanýmý için ayraç olarak verilen deðiþken.
	// strtok fonksiyonu verilen diziyi verilen ayraç iþaretine göre parçalara böler ve her bir parçaya
	// eriþimi mümkün kýlar. bu örnekte dosyadan okuma yapmak için bu fonksiyon çok uygundur.
	while (fgets (line, n, fp) != NULL)
	// deðerler satýr satýr okunur.
	{
		char *ptr = strtok(line, delim);
		m = addtomov(ptr);
		// iþaret gelmeden önce ilk kelime film. film olarak alýnýr ve film dizisine eklenir.
		
		char com[200][50]; // bu dizi mevcut satýrdaki tüm oyuncularý string olarak tutar.
		
		i = 0;
		ptr = strtok(NULL, delim);
		while (ptr != NULL)
		{
			strcpy(com[i], ptr);
			ptr = strtok(NULL, delim);
			i++;
			// bu iþlemle tüm oyuncularý com dizisine attýk
		}
		
		int indisdizisi[200]; // bu dizi com dizisine aldýðýmýz tüm oyuncularýn grapha eklenecek indis deðerlerini tutar.
		for (j = 0; j < i; j++)
		{
			indisdizisi[j] = addtoarray(com[j]);
			// bu döngü tek tek tüm kelimeleri static diziye yolluyor ve indislerini kaydediyor.
		}
		
		for (j = 0; j < i; j++)
		{
			// bu döngüde kaydettiðimiz indisler, movie de dikkate alýnarak tüm 2 li kombinasyonlar þeklinde grapha ekleniyor.
			x = indisdizisi[j];
			for (k = j+1; k < i; k++)
			{
				y = indisdizisi[k];
				graphekle(graph, x, y, m);
			}
		}
		char line[10000];
	}
	
	int baconarray = addtoarray("Bacon, Kevin");
	// Kevin baconun indisini bulduk.
	
	bfs(graph, baconarray);
	// bu indisi bfs algoritmasýna göndererek algoritmayý çalýþtýrdýk.
	
	herbirbaconsayisi();
	// bfs algoritmasý ardýndan her kevinbacon sayýsýna ait kaç kiþi olduðunu yazan fonksiyon.
	
	printf("\n\n\n\n");
	char oyuncu[30];
	char stop[] = "STOP";
	while (strcmp(oyuncu, stop))
	// Kullanýcý "STOP" girene kadar kullanýcýdan oyuncu ismi isteyen fonksiyon. "a-zA-Z., " dýþýnda karakterler çalýþmaz.
	{
		printf("\n\nOyuncu Ismini Giriniz (Type: 'STOP' to stop): ");
		char t = getchar(); // entera basýldýðýnda scanf in atlanmamasý için bir karakterlik atlama.
		scanf("%[a-zA-Z., ]s", oyuncu);
		kevinbaconsayisi(graph, oyuncu); // girilen oyunun kevin bacon sayýsý kaç ve oraya nasýl ulaþýldý yazan fonksiyon.
	}
	
	fclose(fp);
	return 0; 
} 

