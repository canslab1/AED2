/*-�����ϼҦ�-*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>   
#include <string>
#include <cmath>
#include <algorithm>
#include <time.h>
/*-�O�_����ɳ��I�y��-*/
#define x1  295265.781						 /*-���W��x�y��-*/ 
#define x2  316372.875						 /*-�k�U��x�y��-*/ 
#define y1 2761738.500						 /*-���W��y�y��-*/ 
#define y2 2789378.999						 /*-�k�U��y�y��-*/ 
/*-�ѼƳ]�w-*/
#define grid              45                 /*-�������-*/
#define store            677                 /*-�O�_���K�Q�ө��ƶq-*/
#define fire              44                 /*-�O�_���������ƶq-*/
#define patient         1625                 /*-�O�_��2010�~OHCA�f�Ҽ�-*/
#define s_distance       300                 /*-��������ϥi�F�Z��-*/
#define limit            100                 /*-���t�m��AED�ƶq-*/  
#define simulation         1                 /*-��������-*/ 
#define generation     10000                 /*-�@�N����-*/ 
#define population       400                 /*-�V����ƥءA�����O4������-*/         
#define rate_selection     0.5               /*-��ܲv-*/ 
#define rate_crossover     0.8               /*-��t�v-*/
/*-�`�Υ���-*/ 
#define FLIP(a)            (((rand() % 1000) / 1000.0) <= (a))
#define RANDOM(x)		   (rand() % (x))
#define Len_x              ((x2) - (x1))
#define Len_y			   ((y2) - (y1))
#define grid_x             (int(Len_x / grid) + 1)
#define grid_y             (int(Len_y / grid) + 1)

using namespace std;
/*-�����ܼ�-*/
double p_data[patient][2];                   /*-�O�_��2010�~OHCA�y�и��, p_data[OHCA�s��][0]:x�b�y��, p_data[OHCA�s��][1]:y�b�y��-*/ 
double f_data[fire][2];                      /*-�O�_���������y�и��, f_data[�������s��][0]:x�b�y��, f_data[�������s��][1]:y�b�y��-*/   
double s_data[store][2];                     /*-�O�_��7-ELEVEN�y�и��, s_data[�ө��s��][0]:x�b�y��, s_data[�ө��s��][1]:y�b�y��-*/      

short int store_gridNum[store][2];           /*-�C��7-ELEVEN����s����m, store_gridNum[�ө��s��][0]:X����s��, store_gridNum[�ө��s��][1]:Y�ө��s��-*/
short int patient_gridNum[patient][2];       /*-�C��OHCA������s����m, patient_gridNum[OHCA�s��][0]:X����s��, patient_gridNum[OHCA�s��][1]:Y����s��-*/ 
short int coverNum[store];                   /*-�C��7-ELEVEN�[�\��OHCA�ƶq-*/

/*-Ū�JOHCA�B�K�Q�ө��ή��������y�и��-*/
bool read_locationData (const char *xfname, const char *yfname, double data[][2], int size, const char *msg)
{       
    ifstream finx(xfname), finy(yfname);
    
    if (!finx || !finy) {
    	cout << "�L�kŪ�J" << msg << "���y�и��\n";
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
/*-�ഫ��ƨüg�J�������G��-*/
void data_transfor (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, 
                    const char *t5fname, const char *t6fname, const char *t7fname, const char *t8fname)
{
	/*-�R���������P��OHCA���-*/
	for (int i = 0; i < patient; i++)
        for (int j = 0; j < fire; j++)
            if (s_distance > sqrt(abs((p_data[i][0] - f_data[j][0]) * (p_data[i][0] - f_data[j][0]) +
			                          (p_data[i][1] - f_data[j][1]) * (p_data[i][1] - f_data[j][1])))) {
                p_data[i][0] = p_data[i][1] = 0;
                break;
            }
    /*-�p��C�Ӻ��檺OHCA�ƶq-*/      
    short int grid_patientNum[grid_x][grid_y];
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            grid_patientNum[i][j] = 0;
    /*-�p��C��OHCA�f�ҩҦb������y�� (x, y)-*/
    for (int x_data = 0, y_data = 0, i = 0; i < patient; i++, x_data = y_data = 0) {
        if (p_data[i][0] != 0 && p_data[i][1] != 0) { 
            x_data = (int)((p_data[i][0] - x1) / grid);
            y_data = (int)((p_data[i][1] - y1) / grid);
            grid_patientNum[x_data][y_data]++;          //�p��C�Ӻ����m��OHCA�f�Ҽƥ� 
        }
        patient_gridNum[i][0] = x_data;                 //�s�J�C��OHCA�f�Ҫ�x�b����y��
        patient_gridNum[i][1] = y_data;                 //�s�J�C��OHCA�f�Ҫ�Y�b����y��
    } 
    /*-�p��C���K�Q�ө��Ҧb������y�� (x, y)-*/ 
    for (int x_data = 0, y_data = 0, i = 0; i < store; i++, x_data = y_data = 0) {
        if (s_data[i][0] != 0 && s_data[i][1] != 0) {
            x_data = (int)((s_data[i][0] - x1) / grid);
            y_data = (int)((s_data[i][1] - y1) / grid);
        }
        store_gridNum[i][0] = x_data;                   //�s�J�C���K�Q�ө���x�b����y��
        store_gridNum[i][1] = y_data;                   //�s�J�C���K�Q�ө���y�b����y��
    }    
    /*-�g��-*/ 
    FILE *fptr;
    fptr = fopen(t1fname, "w"); /*-�ɮ�1:�g�J���Q�R��OHCA����s�����,�ɮצW��:"save_patient.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\t%d\n", patient_gridNum[i][0], patient_gridNum[i][1]);
    fclose(fptr);
	           
    fptr = fopen(t2fname, "w"); /*-�ɮ�2:�g�J�C�ӳ]�I������X����s���A�ɮצW��:"store_gridNumx.txt"-*/
    for (int i = 0; i < store; i++)
        fprintf(fptr, "%d\n", (int)store_gridNum[i][0]);    
    fclose(fptr);           
    
    fptr = fopen(t3fname, "w"); /*-�ɮ�3:�g�J�C�ӳ]�I������Y����s���A�ɮצW��:"store_gridNumy.txt"-*/
    for (int i = 0; i < store; i++)
        fprintf(fptr, "%d\n", (int)store_gridNum[i][1]);    
    fclose(fptr);
    
    fptr = fopen(t4fname, "w"); /*-�ɮ�4:�g�J�C�Ӻ���cover��OHCA�ƶq�A�ɮצW��:"grid_patientNum.txt"-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++, fprintf(fptr, "\n"))
            fprintf(fptr, "%d\t", (int)grid_patientNum[i][j]);
    fclose(fptr);
    
    fptr = fopen(t5fname, "w"); /*-�ɮ�5:�g�J�C��OHCA�Ҧb������s���A�ɮצW��:"patient_gridNumx.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\n", (int)patient_gridNum[i][0]);
    fclose(fptr);
    
    fptr = fopen(t6fname, "w"); /*-�ɮ�6:�g�J�C��OHCA�Ҧb������s���A�ɮצW��:"patient_gridNumy.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%d\n", (int)patient_gridNum[i][1]);
    fclose(fptr);
    
    fptr = fopen(t7fname, "w"); /*-�ɮ�7:�g�J���Q�R��OHCA�y�и��,�ɮצW��:"patient_saveDatax.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%f\n", p_data[i][0]);
    fclose(fptr);
	  
    fptr = fopen(t8fname, "w"); /*-�ɮ�8:�g�J���Q�R��OHCA�y�и��,�ɮצW��:"patient_saveDatay.txt"-*/
    for (int i = 0; i < patient; i++)
        fprintf(fptr, "%f\n", p_data[i][1]);
    fclose(fptr);    
}
/*-��]�t��k���O-*/ 
class geneticAlgorithm
{
    private:
        short int patient_saveNum;                         /*-�L�o�������P��᪺�f�H�`��-*/              
        
        double phenotype[population];                      /*-�C�����餺���]�I��OHCA�_�d�����-*/   
        double fitness[population];                        /*-�C���V���骺�A����-*/
        short int OHCANum[population];                     /*-�C���V����[�\��OHCA�ƥ�-*/
        short int cover_gridNum[population];               /*-�C���V�����л\������ƥ�-*/
        short int individual[population][limit + 3];       /*-��l�ڸs(�V�����)X(�V�������)-*/
        short int store_coverNum[population][limit];       /*-�C���V���餺���]�I�[�\��OHCA��-*/
        double store_coverWeight[population][limit];       /*-�C���V���餺���]�I�[�\��OHCA�_�d�v����-*/

        short int store_coverNum_best[simulation][limit];  /*-�̨άV����]�I�ӧO��OHCA��-*/
        double store_coverWeight_best[simulation][limit];  /*-�̨άV����]�I�ӧO���_�d�����-*/
        
        double fitness_avg[simulation][generation];        /*-�C�ӥ@�N�������V����A����-*/
        double fitness_best[simulation][generation];       /*-�C�ӥ@�N�̦n���V����A����-*/
        double phenotype_best[simulation][generation];     /*-�C�ӥ@�N�̦n���V�����{��-*/
        short int OHCANum_best[simulation][generation];    /*-�C�ӥ@�N�̦n���V����cover��OHCA��-*/
        short int coverGrid[simulation][generation];       /*-�C�ӥ@�N�̨Ϊ��V����cover�������-*/
        
        short int individual_best[simulation][limit + 3];  /*-�̦n���V����-*/
        double select_patientSet[simulation][patient][2];  /*-�D�襤���f�H�s��-*/
        
        int store_gridNum[store][2];                       /*-�C��7-ELEVEN��x_����MY_����-*/
        int patient_gridNum[patient][2];                   /*-�C��OHCA��x_����MY_����-*/

        double rate_mutation;                              /*-���ܲv-*/
        
    public:
    	/*-��]�t��k���O��������ƫŧi-*/ 
        double retrieve_phenotype (int k);                 /*-���o��{��(phenotype)-*/
        bool initialization (void);                        /*-��l��-*/
        void evaluation (int x, int& y);                   /*-����-*/
        void selection (void);                             /*-���-*/
        void crossover (int x[], int y[]);                 /*-���I��t-*/
        void mutation (int x[]);                           /*-����-*/
        void reproduction (void);                          /*-���s-*/    
        void genetic_operation (int x);                    /*-��]�ާ@-*/           
                                                           /*-�g��-*/
        int writeFile (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, const char *t5fname, 
		               const char *t6fname, const char *t7fname, const char *t8fname, const char *t9fname, const char *t10fname);
};
/*-��l�ƨ禡-*/  
bool geneticAlgorithm::initialization (void)
{
    srand(time(NULL));   
    /*-Ū�J�C��OHCA�Ҧb��x����My����s���A�ɮצW��:1.patient_gridNumx.txt 2.patient_gridNumy.txt-*/ 
	ifstream fin1("patient_gridNumx.txt"), fin2("patient_gridNumy.txt");	 
    if (!fin1 || !fin2) {
        cout << "�L�kŪ�J�ɮ�\n"; 
        return false;
	}
    for (int i = 0; i < patient; i++) {
        fin1 >> patient_gridNum[i][0];
        fin2 >> patient_gridNum[i][1];
    }
    fin1.close();
    fin2.close();
    /*-�p�⥼�Q������cover��OHCA��-*/
    for (int i = 0; i < patient; i++)
        if (patient_gridNum[i][0] != 0 && patient_gridNum[i][1] != 0)
        	patient_saveNum++;
    /*-�L�X-*/
    cout << "��b����� : " << setw(4) << grid_x                    << endl;
    cout << "��b����� : " << setw(4) << grid_y                    << endl;     
    cout << "�R���f�Ҽ� : " << setw(4) << patient - patient_saveNum << endl;
    cout << "�O�d�f�Ҽ� : " << setw(4) << patient_saveNum           << endl;

    for (int j = 0; j < store; j++)
        coverNum[j] = 0;            
    /*-���ͪ�N�ڸs�A�H������7-ELEVEN�s���A��l�ƨC���V��������}�C-*/
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
        /*-��l�ƬV�����T�X�A����h��-*/ 
        for (int j = limit; j < limit + 3; j++)
            individual[i][j] = RANDOM(2);
        /*-���o�V���餺���]�I��OHCA�_�d����ȡB�]�I�[�\��OHCA�ơB�]�I�л\�������-*/
        retrieve_phenotype(i);
        cout << "�� " << setw(5) << i + 1 << " ���V���骺�_�d�v�����`�M: " << setw(8) << phenotype[i] << " �����: " << setw(6) << cover_gridNum[i] << " �f�H��: " << setw(4) << OHCANum[i] << endl;
    }
    system("pause");
    return true;
}
/*-Ū�J���e��{�̨Ϊ��V������-*/
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
/*-���o�V�����{��-*/       
double geneticAlgorithm::retrieve_phenotype (int k)
{    
    static short int backup_grid_OHCANum[grid_x][grid_y];
    static int store_gridNum[store][2];
    static bool loaded = false;
    
    short int grid_OHCANum[grid_x][grid_y], grid_Count[grid_x][grid_y];
    
    if (!loaded) {
    	/*-Ū�J�C�Ӻ���[�\��OHCA�ƥ��ɮש�Jgrid_OHCANum�A�ɮצW��:grid_patientNum.txt-*/
    	ifstream fin("grid_patientNum.txt");
    	if (!fin) {
        	cout << "�L�kŪ�J�ɮ�" << endl; 
        	return 0;
    	} 
    	for (int i = 0; i < grid_x; i++)
        	for(int j = 0; j < grid_y; j++)
            	fin >> backup_grid_OHCANum[i][j];
    	fin.close();
    	/*-Ū�J�C�ӳ]�I��X����MY����s��-*/   	
    	ifstream finx("store_gridNumx.txt"), finy("store_gridNumy.txt"); 
    	if (!finx || !finy)
        	cout << "�L�kŪ�J�ɮ�\n"; 
    	for (int i = 0; i < store; i++) {
        	finx >> store_gridNum[i][0];
        	finy >> store_gridNum[i][1];
    	}
    	finx.close();
    	finy.close();
    	
    	loaded = true;
	}
    /*-�ƻsbackup_grid_OHCANum��grid_OHCANum�Pgrid_Count�A�p��]�I�A�Ƚd���|�������-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            grid_Count[i][j] = grid_OHCANum[i][j] = backup_grid_OHCANum[i][j];
    /*-�ܼƫŧi-*/   
    double p_weightSum[limit];         					//�C�ӳ]�I�л\�d�򤺪�OHCA*�_�d�v���� 
    int p_sum     = 0;                           		//�C���V����OHCA���v���`�X 
    int gridSum   = 0;                           	 	//�����]�I�л\�������`�� 
    int gridLevel = individual[k][limit    ] * 4 + 
	                individual[k][limit + 1] * 2 + 
					individual[k][limit + 2] * 1 + 1;   //����h�� 
	for (int i = 0; i < limit; i++) 
		p_weightSum[i] = 0;
    /*-�p��]�I�л\����OHCA�f�� * �Z���v���ȡB OHCA�f�ҼơB�����л\��-*/
    for (int z = 0; z <= 8; z++)
        for (int i = 0; i < limit; i++) {
            int no  = individual[k][i];               	//�]�I�s�� 
            int s_x = store_gridNum[no][0];         	//��e�]�I�Ҧb��x����s�� 
            int s_y = store_gridNum[no][1];         	//��e�]�I�Ҧb��y����s��  
            if (z <= gridLevel)
               for (int x = -(gridLevel); x <= gridLevel; x++)
                   for (int y = -(gridLevel); y <= gridLevel; y++)
                       if (s_x + x >= 0 && s_y + y >= 0) {
                           int m = abs(x) + abs(y);
                           if (m == z) {
                               if(z == 0)                                               //��0�h�A�_�d�v���Ȭ�0.9 
                                   p_weightSum[i] += (double)grid_OHCANum[s_x + x][s_y + y] * (1.0 - (double)((z + 1) * 0.1));
                               else                                                     //1��8�h�A�_�d�v���Ȥ��� :1-0.1*����h��  
                                   p_weightSum[i] += (double)grid_OHCANum[s_x + x][s_y + y] * (1.0 - (double)(z * 0.1));
                               store_coverNum[k][i] += grid_OHCANum[s_x + x][s_y + y];  //�C�ӳ]�I�[�\��OHCA�� 
                               p_sum                += grid_OHCANum[s_x + x][s_y + y];  //�C���V����OHCA���v���`�X
                               grid_OHCANum[s_x + x][s_y + y] =  0;                     //�p��L�����椺OHCA�ƧR�� 
                               grid_Count[s_x + x][s_y + y]   = -1;                     //�w�л\�����欰-1 
                           }
                       }     
        }    
    /*-�p��Ҧ��]�I�л\�������`��-*/
    for (int i = 0; i < grid_x; i++)
        for (int j = 0; j < grid_y; j++)
            if (grid_Count[i][j] == -1)
				gridSum++;
    double avg_coverNum = 0; 
    /*-�C���V���餺���]�I���_�d�v�����`�M-*/      
    for (int i = 0; i < limit; i++)
        if (store_coverNum[k][i] != 0) {
            avg_coverNum            += ((p_weightSum[i]) / (double)store_coverNum[k][i]); 
            store_coverWeight[k][i]  = ((p_weightSum[i]) / (double)store_coverNum[k][i]);
        } 
    phenotype[k]     = avg_coverNum; //�C���V���餺���]�I���_�d�v�����`�M 
    OHCANum[k]       = p_sum;        //�C���V���骺�[�\OHCA�� 
    cover_gridNum[k] = gridSum;      //�C���V���骺�A���л\����� 
}
/*-������x�������B��y���@�N���V���骺fitness score-*/
void geneticAlgorithm::evaluation (int x, int& y)
{
    /*-Ū�J�O�d���f�H�y��-*/
    static double OHCA_setData[patient][2];
    static int m_count, store_gridNum[store][2];
    static bool loaded = false;
    
    if (!loaded) {
    	ifstream finx1("patient_saveDatax.txt"), finy1("patient_saveDatay.txt");
    	if (!finx1 || !finy1) {
        	cout << "�L�kŪ�J�f�H�y��\n"; 
        	exit(-1);
    	} 
    	for (int i = 0; i < patient; i++) {
        	finx1 >> OHCA_setData[i][0];
        	finy1 >> OHCA_setData[i][1];
    	}     
    	finx1.close();
    	finy1.close();
    	/*-Ū�J�C��OHCA��X����MY����s��-*/
    	ifstream finx2("patient_gridNumx.txt"), finy2("patient_gridNumy.txt"); 
    	if (!finx2 || !finy2) {
        	cout << "�L�kŪ�J�ɮ�\n"; 
			exit(-1);
    	} 
    	for (int i = 0; i < patient; i++) {
        	finx2 >> patient_gridNum[i][0];
        	finy2 >> patient_gridNum[i][1];
    	}
    	finx2.close();
    	finy2.close();
    	/*-Ū�J�C�ӳ]�I��X����MY����s��-*/    	
    	ifstream finx3("store_gridNumx.txt"), finy3("store_gridNumy.txt"); 
    	if (!finx3 || !finy3) { 
        	cout << "�L�kŪ�J�ɮ�\n"; 
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
    /*-�p��C���V���骺fitness value�Afitness value = �����]�I��OHCA�_�d����� * �A�Ⱥ����л\�v * OHCA�[�\�v-*/ 
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
    /*-�̷ӬV����A���Ȥj�p�Ƨ�-*/ 
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
    /*-�Z�ʾ���-*/
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
					individual[0][limit + 2] * 1 + 1; 		/*-���@�N�̨Ϊ�����h��-*/
    int g = (int)((gridLevel * 2 + 1) *
	              (gridLevel * 2 + 1) / 2) + 1;  			/*-���@�N�̨Ϊ�����h�Ʃ��л\�������-*/				      
    fitness_best[x][y]   = fitness[0];    					/*-���@�N�̨Ϊ��V����fitness value-*/        
    phenotype_best[x][y] = phenotype[0];					/*-���@�N�̨Ϊ��]�I��OHCA�_�d�����-*/ 
    OHCANum_best[x][y]   = OHCANum[0];						/*-���@�N�̨Ϊ�OHCA�[�\��-*/ 
    coverGrid[x][y]      = cover_gridNum[0];				/*-���@�N�̨Ϊ������л\��-*/
	    
    cout << "�̨ξA����:"           << setw(10) << fitness_best[x][y]   << "   " 
	     << "��{��:"               << setw( 8) << phenotype_best[x][y] << "   " 
		 << "�����:"               << setw( 5) << cover_gridNum[0]     << "   " 
		 << "�f�H��:"               << setw( 4) << OHCANum_best[x][y]   << endl;
    cout << "����h��:"             << setw( 3) << gridLevel            << "   "
	     << "�C��7-ELEVEN cover��:" << setw( 3) << g                    << "   "
	     << "������:"               << setw( 3) << m_count              << endl; 
    /*-���@�N�̨Ϊ��V����s�X-*/ 
    for (int i = 0; i < limit + 3; i++)
        individual_best[x][i] = individual[0][i];
    /*-���@�N�̨Ϊ��V���餺�C�ӳ]�I�Ҳ[�\��OHCA�ƻPOHCA�_�d�����-*/
    for (int i = 0; i < limit; i++) {
        store_coverNum_best[x][i]    = store_coverNum[0][i];
        store_coverWeight_best[x][i] = store_coverWeight[0][i];
    }
    /*-���@�N�̨Ϊ��V����[�\��OHCA�y��-*/
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
    /*-���@�N������fitness value-*/
    for(int i = 0; i < population; i++)
        fitness_sum += fitness[i];
    /*-�C���������C�ӥ@�N������fitness value-*/ 
    fitness_avg[x][y] = fitness_sum / (double)population;
} 
/*-���I��t-*/
void geneticAlgorithm::crossover (int x[], int y[])
{ 
    if FLIP(rate_crossover) {
    	int count = 0;
		for (int i = 0; i < limit - 1; i++)
        	for (int j = i + 1; j < limit; j++)
        		//���p���ۦP��]�A���e��    
            	if (x[i] == y[j]) {
                	std::swap(x[count], x[i]);
                	std::swap(y[count], y[j]);
                	count++; 
            	} 
    	/*-��t�A�H�����ͤ��I�A���I���᪺��]����-*/  
        for (int i = RANDOM(limit - count) + count; i < limit; i++)
            std::swap(x[i], y[i]);
        for (int i = RANDOM(3) + limit; i < limit + 3; i++)
            std::swap(x[i], y[i]);
    } 
}       
/*-����-*/
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
    /*-��T�X����-*/   
    for (int i = limit; i < limit + 3; i++)
        if FLIP(rate_mutation) 
			x[i] = (x[i] + 1) % 2;
}        
/*-���s-*/
void geneticAlgorithm::reproduction (void)
{ 
    static int survival = (int)((1.0 - rate_selection) * (double)population);	/*-�p���ܤ��V����ƥ�-*/ 

	for (int j = survival; j < population; j += 2) {
        /*-�D��-*/ 
		int parent1 = RANDOM(survival), parent2;
        while (parent1 == (parent2 = RANDOM(survival)))
			;
        int child1[limit + 3], child2[limit + 3];
        /*-�ƻs-*/
        for (int k = 0; k < limit + 3; k++) {
            child1[k] = individual[parent1][k];
            child2[k] = individual[parent2][k];
        }
        /*-��t-*/
        crossover(child1, child2);
        /*-����-*/
        mutation(child1);
        mutation(child2);
        /*-��^�ڸs���A�çR�����O�d���V���骺�������-*/
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
        /*-���o�V���餺���]�I��OHCA�_�d����ȡB�]�I�[�\��OHCA�ơB�]�I�л\�������-*/
        retrieve_phenotype(j);
        retrieve_phenotype(j + 1);
    }
}        
/*-��]�ާ@-*/	
void geneticAlgorithm::genetic_operation (int x)
{       
    for (int i = 0; i < generation; i++) {
        cout << "�� " << setw(5) << i + 1 << " �N" << endl; 
        evaluation(x, i);
        reproduction();          
    }        
}  
/*-�g��-*/      
int geneticAlgorithm::writeFile (const char *t1fname, const char *t2fname, const char *t3fname, const char *t4fname, const char *t5fname, 
                                 const char *t6fname, const char *t7fname, const char *t8fname, const char *t9fname, const char *t10fname)
{
    cout << "�g��" << endl;   
    FILE *fptr;
    fptr = fopen(t1fname, "w"); /*-�g��1: �g�J100�������A1000�ӥ@�N���C�ӳ̦n����{���A�ɮצW��: file_1.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (phenotype_best[i][j] != 0)
            	fprintf(fptr, "%f \n", phenotype_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t2fname, "w"); /*-�g��2: �g�J100�������A1000�ӥ@�N������fitness value�A�ɮצW��: file_2.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (fitness_avg[i][j] != 0)
            	fprintf(fptr, "%f\n", fitness_avg[i][j]);
    fclose(fptr);
    
    fptr = fopen(t3fname, "w"); /*-�g��3: �g�J100�������A1000�ӥ@�N���C�ӳ̦n��fitness value�A�ɮצW��: file_3.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (fitness_best[i][j] != 0)
            	fprintf(fptr, "%f\n", fitness_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t4fname, "w"); /*-�g��4: �g�J�D��X��7-11�s���A�ɮצW��: file_4.txt-*/
    for (int i = 0; i < simulation; i++)
        for (int j = 0; j < limit; j++)
            for (int k = j + 1; k < limit; k++)
                if (individual_best[i][j] > individual_best[i][k])
                    std::swap(individual_best[i][j], individual_best[i][k]);
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit + 3; j++)
            fprintf(fptr, "%d\t", (int)individual_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t5fname, "w"); /*-�g��5: �g�J�D��X��7-11���y�СA�ɮצW��: file_5.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%f\t%f\n", s_data[(int)(individual_best[i][j])][0], s_data[(int)(individual_best[i][j])][1]);
    fclose(fptr);
    
    fptr = fopen(t6fname, "w"); /*-�g��6: �g�J�D��X�̦n���t�m�a�I�Ҳ[�\���d��A�ɮצW��: file_6.txt-*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (coverGrid[i][j] != 0)
            	fprintf(fptr, "%d\n", coverGrid[i][j]);
    fclose(fptr);
    
    fptr = fopen(t7fname, "w"); /*-�g��7: �g�Jn�������A1000�ӥ@�N���C�ӳ̦n���f�H�ơA�ɮצW��: file_7.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < generation; j++)
        	if (OHCANum_best[i][j] != 0)
            	fprintf(fptr, "%d\n", OHCANum_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t8fname, "w"); /*-�g��8: �g�Jn�������A�̨άV���餺�]�I�[�\��OHCA�ơA�ɮצW��: file_8.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%d\t", store_coverNum_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t9fname,"w"); /*-�g��9: �g�Jn�������A�̨άV���餺�]�I�[�\��OHCA�_�d����ȡA�ɮצW��: file_9.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < limit; j++)
            fprintf(fptr, "%f\t", store_coverWeight_best[i][j]);
    fclose(fptr);
    
    fptr = fopen(t10fname, "w"); /*-�g��10: �g�Jn�������A1000�ӥ@�N���C�ӳ̦n���f�H�Ʈy�СA�ɮצW��: file_10.txt--*/
    for (int i = 0; i < simulation; i++, fprintf(fptr, "\n"))
        for (int j = 0; j < patient; j++)
            if (select_patientSet[i][j][0] != 0 && select_patientSet[i][j][1] != 0)
                fprintf(fptr, "%f\t%f\n", select_patientSet[i][j][0], select_patientSet[i][j][1]);
    fclose(fptr);            
} 

int main(void)
{
    clock_t start = clock();
    if (read_locationData(  "99_x.txt",   "99_y.txt", p_data, patient, "�f�H") &&
    	read_locationData( "711_x.txt",  "711_y.txt", s_data, store,   "�K�Q�ө�") &&
    	read_locationData("Fire_x.txt", "Fire_y.txt", f_data, fire,    "������")) {
    	data_transfor(    "save_patient.txt",   "store_gridNumx.txt",    "store_gridNumy.txt",   "grid_patientNum.txt", 
	    	          "patient_gridNumx.txt", "patient_gridNumy.txt", "patient_saveDatax.txt", "patient_saveDatay.txt");    
    	for (int i = 0; i < simulation; i++) {
        	cout << "�� " << setw(2) << i + 1 << " �������}�l!!" << endl;
        	geneticAlgorithm g;
        	if (g.initialization()) {
        		g.genetic_operation(i);
        		g.writeFile("file_1.txt", "file_2.txt", "file_3.txt", "file_4.txt", "file_5.txt", 
		    	        	"file_6.txt", "file_7.txt", "file_8.txt", "file_9.txt", "file_10.txt");
			}
    	}
	}
    cout << "����ɶ�: " << double(clock() - start) / 1000.0 << "��" << endl;
    system("pause");
    return 0;
}
