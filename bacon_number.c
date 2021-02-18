#include <stdio.h> 
#include <stdlib.h>
#include <limits.h> 
#include <string.h>


static char indexarray[200000][50]; // Her bir oyuncunun eklendi�i dizi. Oyuncunun bu dizideki indexi grapha eklenir.
static int nextindex = 0; // Yukar�daki diziye oyuncu eklerken s�ray� tutan de�i�ken.
static char indexmov[30000][100]; // Her bir filmin eklendi�i dizi. Dizilerin indexi graphlardaki d���mlerde tutulacak.
static int nextmov = 0; // Yukar�daki diziye film eklerken s�ray� tutan de�i�ken.
static int visited[200000][3]; 
// BFS algoritmas� kullan�l�rken bir d���m ziyaret edildi mi bilgisini tutan, ayr�ca o d���m�n Kevin Bacona uzakl���n� tutan dizi.
// Bu diziyi tek boyuta indirgemeyi denedim ama k���k bir ka� sebepten �t�r� bu �ekilde yapmak daha kolay oldu.
// Ayr�ca graph i�inde de tutulabilirdi. Static olarak tan�mlamak daha kolay olaca�� i�in bu �ekilde yapt�m.
static int path[200000]; // bir d���me hangi d���mden gelindi�ini tutan dizi.
static int baconsum = 0; 
// Ka� hangi uzakl�kta ka� adet d���m varsa hepsinin toplam�n� tutan de�i�ken.
// nextindex - baconsum yaparak ka� adet d���m�n sonsuz uzakl�kta oldu�u elde edilebiliyor.

struct Queue // Kuruk veri yap�s�. �n arka ve dizinin kapasitesi tutuluyor.
{ 
    int front, rear, size; 
    int capacity; 
    int* array; 
}; 

struct Queue* createQueue (int capacity) 
// Bir kapasite verilerek kuyruk olu�turmaya yarayan fonksiyon.
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
// Kuyru�un dolu olup olmad���n� d�ner.
	return (queue->size == queue->capacity);  
} 
  
int bosmu (struct Queue* queue)
// Kuyru�un bo� olup olmad���n� d�ner.
{
	return (queue->size == 0);
} 

void elemanekle (struct Queue* queue, int item)
// Kuyru�a eleman ekleyen fonksiyon.
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
// Grapha eklenecek oyuncular. dest oyuncuya �zel say� de�eri.
// movie,  2  oyuncunun hangi filmle birbirlerine ba�l� oldu�unu tutan de�i�ken.
{ 
	int dest;
	int movie;
	struct node* next;
}; 

struct komsuliste 
// Graph� olu�turan listeler.
{ 
	struct node *head;
}; 

struct Graph 
// V adet listeden olu�an graph. array bu listelerin dizisi.
{ 
	int V; 
	struct komsuliste* array;
}; 

struct node* yeninode (int dest) 
// oyuncuya ait d���m olu�turan fonksiyon.
{ 
	struct node* yeni = (struct node*) malloc(sizeof(struct node)); 
	yeni->dest = dest; 
	yeni->next = NULL;
	return yeni; 
} 

struct Graph* createGraph (int V) 
// Parametre olarak verilen V de�eri b�y�kl���nde Graph olu�turan fonksiyon.
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
// de�erleri verilen 2 oyuncuyu birbirine verilen film ile ba�layan fonksiyon.
// oyuncu d���mlerinde tutulan movie de�erini mov parametresinden al�yor.
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
// ismi verilen oyuncuyu static olarak tan�mlanm�� diziye ekleyen ve dizideki indexini d�nd�ren fonksiyon.
// oyuncu zaten dizideyse direkt olarakdizideki yerini d�nd�r�r.
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
// addtoarray fonksiyonuyla ayn� i�i filmler i�in yapan fonksiyon.
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
// BFS algoritmas�n� uygulayan fonksiyon.
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
// �smi verilen ki�inin kevin bacona ka� film uzakta oldu�unu bulan fonksiyon.
// Ayr�ca bu ki�inin Kevin bacona nas�l ula�t���n� da yazd�r�r.
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
// verilen dosyada hangi kevin bacon say�s�nda ka� ki�i oldu�unu bulan ve yazan fonksiyon.
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
	int V = 200000; // Graph�n b�y�kl��� input-3 dosyas�na uygun olarak 200k atan�r.
	int n = 10000; // bir sat�rda 10000 chara kadar okuma yapmak i�in. 10000 olmas�n�n �zel bir sebebi yok. Y�ksek olmas� �nemli.
	int i = 0; // iterators
	int j = 0;
	int k = 0;
	struct Graph* graph = createGraph(V); // graph isimli graph olu�turuldu.
	
	FILE *fp;
	char dosya[20];
	printf("Name Of  The File (.txt): "); 
	scanf ("%s", dosya);
	fp = fopen(dosya, "r");
	// dosyan�n ismi kullan�c�dan al�nd� ve read modunda a��ld�.
	
	char line[10000]; // t�m sat�r�n tutulaca�� string
	int m;
	int x;
	int y;
	char delim[] = "/"; // strtok fonksiyonu kullan�m� i�in ayra� olarak verilen de�i�ken.
	// strtok fonksiyonu verilen diziyi verilen ayra� i�aretine g�re par�alara b�ler ve her bir par�aya
	// eri�imi m�mk�n k�lar. bu �rnekte dosyadan okuma yapmak i�in bu fonksiyon �ok uygundur.
	while (fgets (line, n, fp) != NULL)
	// de�erler sat�r sat�r okunur.
	{
		char *ptr = strtok(line, delim);
		m = addtomov(ptr);
		// i�aret gelmeden �nce ilk kelime film. film olarak al�n�r ve film dizisine eklenir.
		
		char com[200][50]; // bu dizi mevcut sat�rdaki t�m oyuncular� string olarak tutar.
		
		i = 0;
		ptr = strtok(NULL, delim);
		while (ptr != NULL)
		{
			strcpy(com[i], ptr);
			ptr = strtok(NULL, delim);
			i++;
			// bu i�lemle t�m oyuncular� com dizisine att�k
		}
		
		int indisdizisi[200]; // bu dizi com dizisine ald���m�z t�m oyuncular�n grapha eklenecek indis de�erlerini tutar.
		for (j = 0; j < i; j++)
		{
			indisdizisi[j] = addtoarray(com[j]);
			// bu d�ng� tek tek t�m kelimeleri static diziye yolluyor ve indislerini kaydediyor.
		}
		
		for (j = 0; j < i; j++)
		{
			// bu d�ng�de kaydetti�imiz indisler, movie de dikkate al�narak t�m 2 li kombinasyonlar �eklinde grapha ekleniyor.
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
	// bu indisi bfs algoritmas�na g�ndererek algoritmay� �al��t�rd�k.
	
	herbirbaconsayisi();
	// bfs algoritmas� ard�ndan her kevinbacon say�s�na ait ka� ki�i oldu�unu yazan fonksiyon.
	
	printf("\n\n\n\n");
	char oyuncu[30];
	char stop[] = "STOP";
	while (strcmp(oyuncu, stop))
	// Kullan�c� "STOP" girene kadar kullan�c�dan oyuncu ismi isteyen fonksiyon. "a-zA-Z., " d���nda karakterler �al��maz.
	{
		printf("\n\nOyuncu Ismini Giriniz (Type: 'STOP' to stop): ");
		char t = getchar(); // entera bas�ld���nda scanf in atlanmamas� i�in bir karakterlik atlama.
		scanf("%[a-zA-Z., ]s", oyuncu);
		kevinbaconsayisi(graph, oyuncu); // girilen oyunun kevin bacon say�s� ka� ve oraya nas�l ula��ld� yazan fonksiyon.
	}
	
	fclose(fp);
	return 0; 
} 

