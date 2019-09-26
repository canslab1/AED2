/*-不分區模式-*/
#include <iostream>
#include <fstream>    
#include <cstdlib>
#include <iomanip>   
#include <string>    
#include <conio.h>
#include <cmath>
#include <algorithm>
#include <time.h>
/*-臺北市邊界頂點座標-*/
#define x1  295265.781						 /*-左上角x座標-*/ 
#define x2  316372.875						 /*-右下角x座標-*/ 
#define y1 2761738.500						 /*-左上角y座標-*/ 
#define y2 2789378.999						 /*-右下角y座標-*/ 
/*-參數設定-*/
#define grid              45                 /*-網格邊長-*/
#define store            677                 /*-臺北市便利商店數量-*/
#define fire              44                 /*-臺北市消防局數量-*/
#define patient         1625                 /*-臺北市2010年OHCA病例數-*/
#define s_distance       300                 /*-消防局急救可達距離-*/
#define limit            100                 /*-欲配置的AED數量-*/  
#define simulation         1                 /*-模擬次數-*/ 
#define generation     10000                 /*-世代次數-*/ 
#define population       400                 /*-染色體數目，必須是4的倍數-*/         
#define rate_selection     0.5               /*-選擇率-*/ 
#define rate_crossover     0.8               /*-交配率-*/
/*-常用巨集-*/ 
#define FLIP(a)            (((rand() % 1000) / 1000.0) <= (a))
#define RANDOM(x)	   	   (rand() % (x))
#define Len_x              ((x2) - (x1))
#define Len_y		   	   ((y2) - (y1))
#define grid_x             (int(Len_x / grid) + 1)
#define grid_y             (int(Len_y / grid) + 1)

using namespace std;
/*-全域變數-*/
double p_data[patient][2];                   /*-臺北市2010年OHCA座標資料, p_data[OHCA編號][0]:x軸座標, p_data[OHCA編號][1]:y軸座標-*/ 
double f_data[fire][2];                      /*-臺北市消防局座標資料, f_data[消防局編號][0]:x軸座標, f_data[消防局編號][1]:y軸座標-*/   
double s_data[store][2];                     /*-臺北市7-ELEVEN座標資料, s_data[商店編號][0]:x軸座標, s_data[商店編號][1]:y軸座標-*/      

short int store_gridNum[store][2];           /*-每間7-ELEVEN網格編號位置, store_gridNum[商店編號][0]:X網格編號, store_gridNum[商店編號][1]:Y商店編號-*/
short int patient_gridNum[patient][2];       /*-每個OHCA的網格編號位置, patient_gridNum[OHCA編號][0]:X網格編號, patient_gridNum[OHCA編號][1]:Y網格編號-*/ 
short int coverNum[store];                   /*-每間7-ELEVEN涵蓋的OHCA數量-*/

/*-讀入OHCA、便利商店及消防局的座標資料-*/
bool read_locationData (const char *xfname, const char *yfname, double data[][2], int size, const char *msg)
{       
    ifstream finx(xfname), finy(yfname);
    
    if (!finx || !finy) {
    	cout << "無法讀入" << msg << "的座標資料\n";
    	return false;
    }
    for (int i = 0; i < size; i++) {
        finx >> data[i][0];
        finy >> data[i][1];
    }     
    finx.close();
    finy.close();
    
    return true;
}
/*-轉換資料並寫入中間結果檔-*/
void data_transfor (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, 
                    const char *t5fname, const char *t6fname, const char *t7fname, const char *t8fname)
{
	/*-刪除消防局周圍的OHCA資料-*/
	for (int i = 0; i < patient; i++)
        for (int j = 0; j < fire; j++)
            if (s_distance > sqrt(abs((p_data[i][0] - f_data[j][0]) * (p_data[i][0] - f_data[j][0]) +
			                          (p_data[i][1] - f_data[j][1]) * (p_data[i][1] - f_data[j][1])))) {
                p_data[i][0] = p_data[i][1] = 0;
                break;
            }
    /*-計算每個網格的OHCA數量-*/      
    short int grid_patientNum[grid_x][grid_y];
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            grid_patientNum[i][j] = 0;
    /*-計算每個OHCA病例所在的網格座標 (x, y)-*/
    for (int x_data = 0, y_data = 0, i = 0; i < patient; i++, x_data = y_data = 0) {
        if (p_data[i][0] != 0 && p_data[i][1] != 0) { 
            x_data = (int)((p_data[i][0] - x1) / grid);
            y_data = (int)((p_data[i][1] - y1) / grid);
            grid_patientNum[x_data][y_data]++;          //計算每個網格位置的OHCA病例數目 
        }
        patient_gridNum[i][0] = x_data;                 //存入每個OHCA病例的x軸網格座標
        patient_gridNum[i][1] = y_data;                 //存入每個OHCA病例的Y軸網格座標
    } 
    /*-計算每間便利商店所在的網格座標 (x, y)-*/ 
    for (int x_data = 0, y_data = 0, i = 0; i < store; i++, x_data = y_data = 0) {
        if (s_data[i][0] != 0 && s_data[i][1] != 0) {
            x_data = (int)((s_data[i][0] - x1) / grid);
            y_data = (int)((s_data[i][1] - y1) / grid);
        }
        store_gridNum[i][0] = x_data;                   //存入每間便利商店的x軸網格座標
        store_gridNum[i][1] = y_data;                   //存入每間便利商店的y軸網格座標
    }    
    /*-寫檔-*/ 
    FILE *fptr;
    fptr = fopen(t1fname, "w"); /*-檔案1:寫入未被刪除OHCA網格編號資料,檔案名稱:"save_patient.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\t%d\n", patient_gridNum[i][0], patient_gridNum[i][1]);
    fclose(fptr);
	           
    fptr = fopen(t2fname, "w"); /*-檔案2:寫入每個設施對應的X網格編號，檔案名稱:"store_gridNumx.txt"-*/
    for (int i = 0; i < store; i++)
        fprintf(fptr, "%d\n", (int)store_gridNum[i][0]);    
    fclose(fptr);           
    
    fptr = fopen(t3fname, "w"); /*-檔案3:寫入每個設施對應的Y網格編號，檔案名稱:"store_gridNumy.txt"-*/
    for (int i = 0; i < store; i++)
        fprintf(fptr, "%d\n", (int)store_gridNum[i][1]);    
    fclose(fptr);
    
    fptr = fopen(t4fname, "w"); /*-檔案4:寫入每個網格cover的OHCA數量，檔案名稱:"grid_patientNum.txt"-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++, fprintf(fptr, "\n"))
            fprintf(fptr, "%d\t", (int)grid_patientNum[i][j]);
    fclose(fptr);
    
    fptr = fopen(t5fname, "w"); /*-檔案5:寫入每個OHCA所在的網格編號，檔案名稱:"patient_gridNumx.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\n", (int)patient_gridNum[i][0]);
    fclose(fptr);
    
    fptr = fopen(t6fname, "w"); /*-檔案6:寫入每個OHCA所在的網格編號，檔案名稱:"patient_gridNumy.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\n", (int)patient_gridNum[i][1]);
    fclose(fptr);
    
    fptr = fopen(t7fname, "w"); /*-檔案7:寫入未被刪除OHCA座標資料,檔案名稱:"patient_saveDatax.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%f\n", p_data[i][0]);
    fclose(fptr);
	  
    fptr = fopen(t8fname, "w"); /*-檔案8:寫入未被刪除OHCA座標資料,檔案名稱:"patient_saveDatay.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%f\n", p_data[i][1]);
    fclose(fptr);    
}
/*-基因演算法類別-*/ 
class geneticAlgorithm
{
    private:
        short int patient_saveNum;                         /*-過濾消防局周圍後的病人總數-*/              
        
        double phenotype[population];                      /*-每條色體內的設施之OHCA復甦期望值-*/   
        double fitness[population];                        /*-每條染色體的適應值-*/
        short int OHCANum[population];                     /*-每條染色體涵蓋的OHCA數目-*/
        short int cover_gridNum[population];               /*-每條染色體覆蓋的網格數目-*/
        short int individual[population][limit + 3];       /*-初始族群(染色體數)X(染色體長度)-*/
        short int store_coverNum[population][limit];       /*-每條染色體內的設施涵蓋的OHCA數-*/
        double store_coverWeight[population][limit];       /*-每條染色體內的設施涵蓋的OHCA復甦權重值-*/

        short int store_coverNum_best[simulation][limit];  /*-最佳染色體設施個別之OHCA數-*/
        double store_coverWeight_best[simulation][limit];  /*-最佳染色體設施個別之復甦期望值-*/
        
        double fitness_avg[simulation][generation];        /*-每個世代平均的染色體適應值-*/
        double fitness_best[simulation][generation];       /*-每個世代最好的染色體適應值-*/
        double phenotype_best[simulation][generation];     /*-每個世代最好的染色體表現型-*/
        short int OHCANum_best[simulation][generation];    /*-每個世代最好的染色體cover的OHCA數-*/
        short int coverGrid[simulation][generation];       /*-每個世代最佳的染色體cover的網格數-*/
        
        short int individual_best[simulation][limit + 3];  /*-最好的染色體-*/
        double select_patientSet[simulation][patient][2];  /*-挑選中的病人編號-*/
        
        int store_gridNum[store][2];                       /*-每間7-ELEVEN的x_網格和Y_網格-*/
        int patient_gridNum[patient][2];                   /*-每個OHCA的x_網格和Y_網格-*/

        double rate_mutation;                              /*-突變率-*/
        
    public:
    	/*-基因演算法類別之成員函數宣告-*/ 
        double retrieve_phenotype (int k);                 /*-取得表現型(phenotype)-*/
        bool initialization (void);                        /*-初始化-*/
        void evaluation (int x, int& y);                   /*-評估-*/
        void selection (void);                             /*-選擇-*/
        void crossover (int x[], int y[]);                 /*-單點交配-*/
        void mutation (int x[]);                           /*-突變-*/
        void reproduction (void);                          /*-重製-*/    
        void genetic_operation (int x);                    /*-基因操作-*/           
                                                           /*-寫檔-*/
        int writeFile (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, const char *t5fname, 
		               const char *t6fname, const char *t7fname, const char *t8fname, const char *t9fname, const char *t10fname);
};
/*-初始化函式-*/  
bool geneticAlgorithm::initialization (void)
{
    srand(time(NULL));   
    /*-讀入每個OHCA所在的x網格和y網格編號，檔案名稱:1.patient_gridNumx.txt 2.patient_gridNumy.txt-*/ 
	ifstream fin1("patient_gridNumx.txt"), fin2("patient_gridNumy.txt");	 
    if (!fin1 || !fin2) {
        cout << "無法讀入檔案\n"; 
        return false;
	}
    for (int i = 0; i < patient; i++) {
        fin1 >> patient_gridNum[i][0];
        fin2 >> patient_gridNum[i][1];
    }
    fin1.close();
    fin2.close();
    /*-計算未被消防局cover的OHCA數-*/
    for (int i = 0; i < patient; i++)
        if (patient_gridNum[i][0] != 0 && patient_gridNum[i][1] != 0)
        	patient_saveNum++;
    /*-印出-*/
    cout << "Ｘ軸網格數 : " << setw(4) << grid_x                    << endl;
    cout << "Ｙ軸網格數 : " << setw(4) << grid_y                    << endl;     
    cout << "刪除病例數 : " << setw(4) << patient - patient_saveNum << endl;
    cout << "保留病例數 : " << setw(4) << patient_saveNum           << endl;

    for (int j = 0; j < store; j++)
        coverNum[j] = 0;            
    /*-產生初代族群，隨機產生7-ELEVEN編號，初始化每條染色體相關陣列-*/
    for (int i = 0; i < population; i++) {
        phenotype[i] = fitness[i]       = 0;
        OHCANum[i]   = cover_gridNum[i] = 0;        
        for (int r, j = 0; j < limit; j++) {
            store_coverNum[i][j]    = 0;
            store_coverWeight[i][j] = 0;
            do { r = RANDOM(store); } while (s_data[r][0] == 0 && s_data[r][1] == 0);
            for (int k = 0; k < j; k++)
                while (r == individual[i][k]) {
                	do { r = RANDOM(store); } while (s_data[r][0] == 0 && s_data[r][1] == 0);
                    k = 0;
                }   
            individual[i][j] = r;
        }
        /*-初始化染色體後三碼，網格層數-*/ 
        for (int j = limit; j < limit + 3; j++)
            individual[i][j] = RANDOM(2);
        /*-取得染色體內的設施之OHCA復甦期望值、設施涵蓋的OHCA數、設施覆蓋的網格數-*/
        retrieve_phenotype(i);
        cout << "第 " << setw(5) << i + 1 << " 條染色體的復甦權重值總和: " << setw(8) << phenotype[i] << " 網格數: " << setw(6) << cover_gridNum[i] << " 病人數: " << setw(4) << OHCANum[i] << endl;
    }
    system("pause");
    return true;
}
/*-讀入之前表現最佳的染色體資料-*/
/* 
void read_VIP_chromosomes (void)
{
    ifstream fin("vip.txt");
    
    if (fin) {
    	for (int p = 0; !fin.eof(); p++)
    		for (int i = 0; i < limit + 3; i++)
        		fin >> individual[p][i];
    	fin.close();           
	}
}
*/
/*-取得染色體表現型-*/       
double geneticAlgorithm::retrieve_phenotype (int k)
{    
    static short int backup_grid_OHCANum[grid_x][grid_y];
    static int store_gridNum[store][2];
    static bool loaded = false;
    
    short int grid_OHCANum[grid_x][grid_y], grid_Count[grid_x][grid_y];
    
    if (!loaded) {
    	/*-讀入每個網格涵蓋的OHCA數目檔案放入grid_OHCANum，檔案名稱:grid_patientNum.txt-*/
    	ifstream fin("grid_patientNum.txt");
    	if (!fin) {
        	cout << "無法讀入檔案" << endl; 
        	return 0;
    	} 
    	for (int i = 0; i < grid_x; i++)
        	for(int j = 0; j < grid_y; j++)
            	fin >> backup_grid_OHCANum[i][j];
    	fin.close();
    	/*-讀入每個設施的X網格和Y網格編號-*/   	
    	ifstream finx("store_gridNumx.txt"), finy("store_gridNumy.txt"); 
    	if (!finx || !finy)
        	cout << "無法讀入檔案\n"; 
    	for (int i = 0; i < store; i++) {
        	finx >> store_gridNum[i][0];
        	finy >> store_gridNum[i][1];
    	}
    	finx.close();
    	finy.close();
    	
    	loaded = true;
	}
    /*-複製backup_grid_OHCANum到grid_OHCANum與grid_Count，計算設施服務範圍重疊的網格數-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            grid_Count[i][j] = grid_OHCANum[i][j] = backup_grid_OHCANum[i][j];
    /*-變數宣告-*/   
    double p_weightSum[limit];         					//每個設施覆蓋範圍內的OHCA*復甦權重值 
    int p_sum     = 0;                           		//每條染色體OHCA的權重總合 
    int gridSum   = 0;                           	 	//全部設施覆蓋的網格總數 
    int gridLevel = individual[k][limit    ] * 4 + 
	                individual[k][limit + 1] * 2 + 
					individual[k][limit + 2] * 1 + 1;   //網格層數 
	for (int i = 0; i < limit; i++) 
		p_weightSum[i] = 0;
    /*-計算設施覆蓋內的OHCA病例 * 距離權重值、 OHCA病例數、網格覆蓋數-*/
    for (int z = 0; z <= 8; z++)
        for (int i = 0; i < limit; i++) {
            int no  = individual[k][i];               	//設施編號 
            int s_x = store_gridNum[no][0];         	//當前設施所在的x網格編號 
            int s_y = store_gridNum[no][1];         	//當前設施所在的y網格編號  
            if (z <= gridLevel)
               for (int x = -(gridLevel); x <= gridLevel; x++)
                   for (int y = -(gridLevel); y <= gridLevel; y++)
                       if (s_x + x >= 0 && s_y + y >= 0) {
                           int m = abs(x) + abs(y);
                           if (m == z) {
                               if(z == 0)                                               //第0層，復甦權重值為0.9 
                                   p_weightSum[i] += (double)grid_OHCANum[s_x + x][s_y + y] * (1.0 - (double)((z + 1) * 0.1));
                               else                                                     //1到8層，復甦權重值公式 :1-0.1*網格層數  
                                   p_weightSum[i] += (double)grid_OHCANum[s_x + x][s_y + y] * (1.0 - (double)(z * 0.1));
                               store_coverNum[k][i] += grid_OHCANum[s_x + x][s_y + y];  //每個設施涵蓋的OHCA數 
                               p_sum                += grid_OHCANum[s_x + x][s_y + y];  //每條染色體OHCA的權重總合
                               grid_OHCANum[s_x + x][s_y + y] =  0;                     //計算過的網格內OHCA數刪除 
                               grid_Count[s_x + x][s_y + y]   = -1;                     //已覆蓋的網格為-1 
                           }
                       }     
        }    
    /*-計算所有設施覆蓋的網格總數-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            if (grid_Count[i][j] == -1)
				gridSum++;
    double avg_coverNum = 0; 
    /*-每條染色體內的設施之復甦權重值總和-*/      
    for (int i = 0; i < limit; i++)
        if (store_coverNum[k][i] != 0) {
            avg_coverNum            += ((p_weightSum[i]) / (double)store_coverNum[k][i]); 
            store_coverWeight[k][i]  = ((p_weightSum[i]) / (double)store_coverNum[k][i]);
        } 
    phenotype[k]     = avg_coverNum; //每條染色體內的設施之復甦權重值總和 
    OHCANum[k]       = p_sum;        //每條染色體的涵蓋OHCA數 
    cover_gridNum[k] = gridSum;      //每條染色體的服務覆蓋網格數 
}
/*-評估第x次模擬、第y次世代的染色體的fitness score-*/
void geneticAlgorithm::evaluation (int x, int& y)
{
    /*-讀入保留的病人座標-*/
    static double OHCA_setData[patient][2];
    static int m_count, store_gridNum[store][2];
    static bool loaded = false;
    
    if (!loaded) {
    	ifstream finx1("patient_saveDatax.txt"), finy1("patient_saveDatay.txt");
    	if (!finx1 || !finy1) {
        	cout << "無法讀入病人座標\n"; 
        	exit(-1);
    	} 
    	for (int i = 0; i < patient; i++) {
        	finx1 >> OHCA_setData[i][0];
        	finy1 >> OHCA_setData[i][1];
    	}     
    	finx1.close();
    	finy1.close();
    	/*-讀入每個OHCA的X網格和Y網格編號-*/
    	ifstream finx2("patient_gridNumx.txt"), finy2("patient_gridNumy.txt"); 
    	if (!finx2 || !finy2) {
        	cout << "無法讀入檔案\n"; 
			exit(-1);
    	} 
    	for (int i = 0; i < patient; i++) {
        	finx2 >> patient_gridNum[i][0];
        	finy2 >> patient_gridNum[i][1];
    	}
    	finx2.close();
    	finy2.close();
    	/*-讀入每個設施的X網格和Y網格編號-*/    	
    	ifstream finx3("store_gridNumx.txt"), finy3("store_gridNumy.txt"); 
    	if (!finx3 || !finy3) { 
        	cout << "無法讀入檔案\n"; 
			exit(-1);
    	} 
    	for (int i = 0; i < store; i++) {
        	finx3 >> store_gridNum[i][0];
        	finy3 >> store_gridNum[i][1];
    	}
    	finx3.close();
    	finy3.close();
    	
    	loaded = true;
	}
    /*-計算每條染色體的fitness value，fitness value = 全部設施的OHCA復甦期望值 * 服務網格覆蓋率 * OHCA涵蓋率-*/ 
    for (int gridLevel, g, i = 0; i < population; i++) {
        gridLevel  = individual[i][limit    ] * 4 + 
		             individual[i][limit + 1] * 2 + 
		  	 		 individual[i][limit + 2] * 1 + 1; 
        g          = (int)((gridLevel * 2 + 1) * 
		                   (gridLevel * 2 + 1) / 2) + 1;
		fitness[i] = (    (double)phenotype[i] / (double)limit) * 
			         ((double)cover_gridNum[i] / (double)(g * limit)) * 
				     (      (double)OHCANum[i] / (double)patient_saveNum);
    } 
    /*-依照染色體適應值大小排序-*/ 
    for (int i = 0; i < population - 1; i++)
        for (int j = i + 1; j < population; j++)
            if (fitness[i] < fitness[j]) {
                std::swap(fitness[i],   fitness[j]);
                std::swap(phenotype[i], phenotype[j]);
                std::swap(OHCANum[i],   OHCANum[j]);             
                for (int k = 0; k < limit + 3; k++)
                    std::swap(individual[i][k], individual[j][k]);
                for (int k = 0; k < limit; k++) {
                    std::swap(store_coverNum[i][k],    store_coverNum[j][k]);
                    std::swap(store_coverWeight[i][k], store_coverWeight[j][k]);
                }
                std::swap(cover_gridNum[i], cover_gridNum[j]);
            }
    /*-擾動機制-*/
    if (y == 0) {
    	m_count       = 0;
    	rate_mutation = 0.01;
	}
    else if(fitness[0] == fitness_best[x][y - 1]) {
        m_count++;
        rate_mutation = ((m_count >= 50) && (m_count % 10 == 0) ? 0.1 : 0.01);
        if (m_count >= 500) 
            y = generation - 1;
    }  
    else { 
        m_count       = 0;
        rate_mutation = 0.01;
    }
    	
    double fitness_sum = 0;     
    int gridLevel = individual[0][limit    ] * 4 + 
	                individual[0][limit + 1] * 2 + 
					individual[0][limit + 2] * 1 + 1; 		/*-此世代最佳的網格層數-*/
    int g = (int)((gridLevel * 2 + 1) *
	              (gridLevel * 2 + 1) / 2) + 1;  			/*-此世代最佳的網格層數所覆蓋的網格數-*/				      
    fitness_best[x][y]   = fitness[0];    					/*-此世代最佳的染色體fitness value-*/        
    phenotype_best[x][y] = phenotype[0];					/*-此世代最佳的設施的OHCA復甦期望值-*/ 
    OHCANum_best[x][y]   = OHCANum[0];						/*-此世代最佳的OHCA涵蓋數-*/ 
    coverGrid[x][y]      = cover_gridNum[0];				/*-此世代最佳的網格覆蓋數-*/
	    
    cout << "最佳適應值:"           << setw(10) << fitness_best[x][y]   << "   " 
	     << "表現型:"               << setw( 8) << phenotype_best[x][y] << "   " 
		 << "網格數:"               << setw( 5) << cover_gridNum[0]     << "   " 
		 << "病人數:"               << setw( 4) << OHCANum_best[x][y]   << endl;
    cout << "網格層數:"             << setw( 3) << gridLevel            << "   "
	     << "每間7-ELEVEN cover數:" << setw( 3) << g                    << "   "
	     << "停滯數:"               << setw( 3) << m_count              << endl; 
    /*-此世代最佳的染色體編碼-*/ 
    for (int i = 0; i < limit + 3; i++)
        individual_best[x][i] = individual[0][i];
    /*-此世代最佳的染色體內每個設施所涵蓋的OHCA數與OHCA復甦期望值-*/
    for (int i = 0; i < limit; i++) {
        store_coverNum_best[x][i]    = store_coverNum[0][i];
        store_coverWeight_best[x][i] = store_coverWeight[0][i];
    }
    /*-此世代最佳的染色體涵蓋的OHCA座標-*/
    int select_patient[patient];
    for (int i = 0; i < patient; i++)
        select_patient[i] = -1;
    for (int i = 0; i < limit; i++) {
    	int no  = individual[0][i];
        int s_x = store_gridNum[no][0], s_y = store_gridNum[no][1]; 
        for (int x = -(gridLevel); x <= gridLevel; x++) 
            for (int y = -(gridLevel); y <= gridLevel; y++) {
                int m = abs(x) + abs(y);
                if (m <= gridLevel)
                    for (int j = 0; j < patient; j++) {
                        int t_x = patient_gridNum[j][0], t_y = patient_gridNum[j][1];
                        if (t_x == (s_x + x) && t_y == (s_y + y))
                            select_patient[j] = 1;
                    } 
            }
    }
    for (int i = 0; i < patient; i++)
        if (select_patient[i] == 1) {
            select_patientSet[x][i][0] = OHCA_setData[i][0];
            select_patientSet[x][i][1] = OHCA_setData[i][1];
        }
        else {
            select_patientSet[x][i][0] = 0;
            select_patientSet[x][i][1] = 0;
        }
    /*-此世代的平均fitness value-*/
    for(int i = 0; i < population; i++)
        fitness_sum += fitness[i];
    /*-每次模擬中每個世代的平均fitness value-*/ 
    fitness_avg[x][y] = fitness_sum / (double)population;
} 
/*-單點交配-*/
void geneticAlgorithm::crossover (int x[], int y[])
{ 
    if FLIP(rate_crossover) {
    	int count = 0;
		for (int i = 0; i < limit - 1; i++)
        	for (int j = i + 1; j < limit; j++)
        		//假如有相同基因，往前移    
            	if (x[i] == y[j]) {
                	std::swap(x[count], x[i]);
                	std::swap(y[count], y[j]);
                	count++; 
            	} 
    	/*-交配，隨機產生切點，切點之後的基因互換-*/  
        for (int i = RANDOM(limit - count) + count; i < limit; i++)
            std::swap(x[i], y[i]);
        for (int i = RANDOM(3) + limit; i < limit + 3; i++)
            std::swap(x[i], y[i]);
    } 
}       
/*-突變-*/
void geneticAlgorithm::mutation (int x[])
{
    for (int t, i = 0; i < limit; i++) {
        if FLIP(rate_mutation) {
        	do { t = RANDOM(store); } while (s_data[t][0] == 0 && s_data[t][1] == 0);
            for (int j = 0; j < limit; j++)
                while (t == x[j]) {
                    j = 0;
                    do { t = RANDOM(store); } while (s_data[t][0] == 0 && s_data[t][1] == 0); 
                }
            x[i] = t;       
        }       
    } 
    /*-後三碼突變-*/   
    for (int i = limit; i < limit + 3; i++)
        if FLIP(rate_mutation) 
			x[i] = (x[i] + 1) % 2;
}        
/*-重製-*/
void geneticAlgorithm::reproduction (void)
{ 
    static int survival = (int)((1.0 - rate_selection) * (double)population);	/*-計算選擇之染色體數目-*/ 

	for (int j = survival; j < population; j += 2) {
        /*-挑選-*/ 
		int parent1 = RANDOM(survival), parent2;
        while (parent1 == (parent2 = RANDOM(survival)))
			;
        int child1[limit + 3], child2[limit + 3];
        /*-複製-*/
        for (int k = 0; k < limit + 3; k++) {
            child1[k] = individual[parent1][k];
            child2[k] = individual[parent2][k];
        }
        /*-交配-*/
        crossover(child1, child2);
        /*-突變-*/
        mutation(child1);
        mutation(child2);
        /*-放回族群中，並刪除未保留之染色體的相關資料-*/
        for (int k = 0; k < limit + 3; k++) {
            individual[j][k]     = child1[k];
            individual[j + 1][k] = child2[k];
            if (k < limit) {
	            store_coverNum[j][k]    = store_coverNum[j + 1][k]    = 0;            
    	        store_coverWeight[j][k] = store_coverWeight[j + 1][k] = 0;
    		}
        }
        phenotype[j]     = phenotype[j + 1]     = 0;
        OHCANum[j]       = OHCANum[j + 1]       = 0;
        fitness[j]       = fitness[j + 1]       = 0;
        cover_gridNum[j] = cover_gridNum[j + 1] = 0;
        /*-取得染色體內的設施之OHCA復甦期望值、設施涵蓋的OHCA數、設施覆蓋的網格數-*/
        retrieve_phenotype(j);
        retrieve_phenotype(j + 1);
    }
}        
/*-基因操作-*/	
void geneticAlgorithm::genetic_operation (int x)
{       
    for (int i = 0; i < generation; i++) {
        cout << "第 " << setw(5) << i + 1 << " 代" << endl; 
        evaluation(x, i);
        reproduction();          
    }        
}  
/*-寫檔-*/      
int geneticAlgorithm::writeFile (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, const char *t5fname, 
                                 const char *t6fname, const char *t7fname, const char *t8fname, const char *t9fname, const char *t10fname)
{
    cout << "寫檔" << endl;   
    FILE *fptr;
    fptr = fopen(t1fname, "w"); /*-寫檔1: 寫入100次模擬，1000個世代中每個最好的表現型，檔案名稱: file_1.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (phenotype_best[i][j] != 0)
            	fprintf(fptr, "%f \n", phenotype_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t2fname, "w"); /*-寫檔2: 寫入100次模擬，1000個世代平均的fitness value，檔案名稱: file_2.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (fitness_avg[i][j] != 0)
            	fprintf(fptr, "%f\n", fitness_avg[i][j]);
    fclose(fptr);
    
    fptr = fopen(t3fname, "w"); /*-寫檔3: 寫入100次模擬，1000個世代中每個最好的fitness value，檔案名稱: file_3.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (fitness_best[i][j] != 0)
            	fprintf(fptr, "%f\n", fitness_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t4fname, "w"); /*-寫檔4: 寫入挑選出的7-11編號，檔案名稱: file_4.txt-*/
    for (int i = 0; i < simulation; i++)
        for (int j = 0; j < limit; j++)
            for (int k = j + 1; k < limit; k++)
                if (individual_best[i][j] > individual_best[i][k])
                    std::swap(individual_best[i][j], individual_best[i][k]);
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit + 3; j++)
            fprintf(fptr, "%d\t", (int)individual_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t5fname, "w"); /*-寫檔5: 寫入挑選出的7-11的座標，檔案名稱: file_5.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%f\t%f\n", s_data[(int)(individual_best[i][j])][0], s_data[(int)(individual_best[i][j])][1]);
    fclose(fptr);
    
    fptr = fopen(t6fname, "w"); /*-寫檔6: 寫入挑選出最好的配置地點所涵蓋的範圍，檔案名稱: file_6.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (coverGrid[i][j] != 0)
            	fprintf(fptr, "%d\n", coverGrid[i][j]);
    fclose(fptr);
    
    fptr = fopen(t7fname, "w"); /*-寫檔7: 寫入n次模擬，1000個世代中每個最好的病人數，檔案名稱: file_7.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (OHCANum_best[i][j] != 0)
            	fprintf(fptr, "%d\n", OHCANum_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t8fname, "w"); /*-寫檔8: 寫入n次模擬，最佳染色體內設施涵蓋的OHCA數，檔案名稱: file_8.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%d\t", store_coverNum_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t9fname,"w"); /*-寫檔9: 寫入n次模擬，最佳染色體內設施涵蓋的OHCA復甦期望值，檔案名稱: file_9.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%f\t", store_coverWeight_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t10fname, "w"); /*-寫檔10: 寫入n次模擬，1000個世代中每個最好的病人數座標，檔案名稱: file_10.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < patient; j++)
            if (select_patientSet[i][j][0] != 0 && select_patientSet[i][j][1] != 0)
                fprintf(fptr, "%f\t%f\n", select_patientSet[i][j][0], select_patientSet[i][j][1]);
    fclose(fptr);            
} 

int main(void)
{
    clock_t start = clock();
    if (read_locationData(  "99_x.txt",   "99_y.txt", p_data, patient, "病人") &&
    	read_locationData( "711_x.txt",  "711_y.txt", s_data, store,   "便利商店") &&
    	read_locationData("Fire_x.txt", "Fire_y.txt", f_data, fire,    "消防局")) {
    	data_transfor(    "save_patient.txt",   "store_gridNumx.txt",    "store_gridNumy.txt",   "grid_patientNum.txt", 
	    	          "patient_gridNumx.txt", "patient_gridNumy.txt", "patient_saveDatax.txt", "patient_saveDatay.txt");    
    	for (int i = 0; i < simulation; i++) {
        	cout << "第 " << setw(2) << i + 1 << " 次模擬開始!!" << endl;
        	geneticAlgorithm g;
        	if (g.initialization()) {
        		g.genetic_operation(i);
        		g.writeFile("file_1.txt", "file_2.txt", "file_3.txt", "file_4.txt", "file_5.txt", 
		    	        	"file_6.txt", "file_7.txt", "file_8.txt", "file_9.txt", "file_10.txt");
			}
    	}
	}
    cout << "執行時間: " << double(clock() - start) / 1000.0 << "秒" << endl;
    system("pause");
    return 0;
}
