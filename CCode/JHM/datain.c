#include "headers.h"
#include "datain.h"


/*TEST*/

int testargc(int argc)
{
 	if (argc!=4) {
    		perror("argc failed");
    		exit(EXIT_FAILURE);
  	}
return 0;
}


/*READ IN*/


int datadouble(char filename[], char filename2[], double datavec[] )
{
	int i;
	char number[20];
	double data;
	FILE *file;
	FILE *file2;
	file=fopen(filename, "r");
	i=0;
	if ( file != NULL ){
		fscanf(file, "%s %lf",number,&data);
		while ( fscanf(file, "%s %lf",number,&data)!=-1){
			datavec[i]=data;   
			i++;
		}
	}
	else{perror(filename);}
	fclose(file);

	file2=fopen(filename2, "r");
	if ( file != NULL ){
		fscanf(file2, "%s %lf",number,&data);
		while ( fscanf(file2, "%s %lf",number,&data)!=-1)
	        {
			datavec[i]=data;
			i++;
		}
	}
	else{perror(filename2);}
	fclose(file2);
return 0;
}

int dataint(char filename[],char filename2[], int datavec[] )
{
	int i;
	char number[20];
	double data;
	FILE *file;
	FILE *file2;
	file = fopen(filename, "r");
	i=0;
	if ( file != NULL ){
		fscanf(file, "%s %lf",number,&data);
		while (fscanf(file, "%s %lf",number,&data)!=-1){
			datavec[i]=data;
			i++;
        	}
    	}
	else{perror(filename);}
	fclose(file);
	file2 = fopen(filename, "r");
  	if ( file2 != NULL ){
		fscanf(file2, "%s %lf",number,&data);
		while ( fscanf(file2, "%s %lf",number,&data)!=-1)
	        {
			datavec[i]=data;
			i++;
	        }
	}
	else{perror(filename2);}
	fclose(file2);
return 0;
}

int dataLMN(char filename[], int *datavecL,int *datavecM,int *datavecN)
{
	int i;
	char number[20];
	double data;
	FILE *file = fopen(filename, "r");
	i=0;
	if ( file != NULL ){
		fscanf(file, "%s %lf",number,&data);
		fscanf(file, "%s %lf",number,&data);
		*datavecL=data;     
		fscanf(file, "%s %lf",number,&data);
		*datavecM=data;
		fscanf(file, "%s %lf",number,&data);
		*datavecN=data;
	}
	else{perror(filename);}
	fclose(file);
return 0;
}

/*INZ*/

int inzstruct_MH(struct_MH *MH)
{
	fillMH(MH);
return 0;
}

int inzstruct_priors(struct_priors *priors)
{
	fillpriors(priors);
return 0;
}

int inzstruct_data(struct_data *data)
{
	long size;
	dataLMN("LMNmaxdata.txt",&data->L,&data->M,&data->N);  
	size=data->L*data->M*data->N*2; /*input from file*/
	data->SHIFTlmn=data->L*data->M*data->N; /*input from file*/
  	data->y=malloc(size*sizeof(double));        /*Cycle with SHIFTlmn*/
        data->x=malloc(size*sizeof(double));        /*Cycle with SHIFTlmn*/
	size=data->L*2;
	data->NoORF=malloc(size*sizeof(double));    /*Cycle with data->L*/
	data->NoSUM=malloc(size*sizeof(double));    /*Cycle with data->L*/
	size=17308*2;/*inputfromfile*/
	data->NoTIME=malloc(size*sizeof(double));   /*Cycle with SHIFTlm*/
	
	if (data->y==NULL||data->x==NULL||data->NoORF==NULL||data->NoSUM==NULL||data->NoTIME==NULL) {
		perror("malloc failed");
    		exit(EXIT_FAILURE);
  	}

	datadouble("ydata.txt","ydata2.txt",data->y);
        datadouble("xdata.txt","xdata2.txt",data->x);
        dataint("NoORFdata.txt","NoORFdata2.txt",data->NoORF);
        dataint("NoTIMEdata.txt","NoTIMEdata2.txt",data->NoTIME);

	filldata(data);
return 0;
}

int inzstruct_para(struct_para *para,struct_data *data)
{
	long size;
	size=data->L*2;
	para->tau_K_cl=malloc(size*sizeof(double));
	para->tau_r_cl=malloc(size*sizeof(double));
	size=17308*2;/*inputfromfile*/
	para->K_clm=malloc(size*sizeof(double));
	para->r_clm=malloc(size*sizeof(double));
	size=data->L;
	para->delta=malloc(size*sizeof(double));
	para->gamma=malloc(size*sizeof(double));
	para->omega=malloc(size*sizeof(double));
	para->K_o_l=malloc(size*sizeof(double));
	para->r_o_l=malloc(size*sizeof(double));
	para->nu_l=malloc(size*sizeof(double));
	size=2;
	para->alpha=malloc(size*sizeof(double));
	para->beta=malloc(size*sizeof(double));
	para->upsilon_c=malloc(size*sizeof(double));

	fillpara(para,data);
return 0;
}

/*FILL*/

int fillMH(struct_MH *MH)
{
	MH->hK=0.1;	MH->accept_K=0;
	MH->hr=0.1;	MH->accept_r=0;
	MH->hnu=0.5;	MH->accept_nu=0;
	MH->hP=0.01;	MH->accept_P=0;  /*h sd; accept=0*/
return 0;
}

int filldata(struct_data *D)
{
	int l;
	l=1;

	D->NoSUM[0]=0;
	for (l=1;l<(D->L);l++){
		D->NoSUM[l]=D->NoSUM[l-1]+D->NoORF[l-1];
	}

	D->NoSUM[D->L]=0;
	for (l=(1+D->L);l<(2*D->L);l++){
		D->NoSUM[l]=D->NoSUM[l-1]+D->NoORF[l-1];
	}

	D->SHIFTmn=D->NoSUM[D->L];/*create mnSHIFT*/
return 0;
}

int fillpara(struct_para *D_para, struct_data *D)
{
int c,l,m,mm;
	/*initials*/
	/*K*/
	for (c=0;c<2;c++){
		for (l=0;l<D->L;l++){
			for (m=0;m<D->NoORF[c*D->L+l];m++){
				mm=D->NoSUM[c*D->L+l]+m;
				D_para->K_clm[mm]=D->y[c*D->SHIFTlmn+l*D->M*D->N + m*D->N + D->NoTIME[mm]-1];
				if(D_para->K_clm[mm]>0){D_para->K_clm[mm]=gsl_sf_log(D_para->K_clm[mm]);}
			}
		}
	}

	for (l=0;l<(2*D->L);l++)          {D_para->tau_K_cl[l]=1/(0.4*0.4);}                  /*Precision*/

	for (l=0;l<D->L;l++)          {D_para->K_o_l[l]=gsl_sf_log(0.25);}        /*LMean*/
	D_para->sigma_K_o=1/(0.6*0.6);               /*Precision*/
	D_para->K_p=gsl_sf_log(0.1);       /*LMean*/

	/*r*/
	for (l=0;l<D->L;l++){
		for (m=0;m<D->NoORF[l];m++){
			mm=D->NoSUM[c*D->L+l]+m;
			D_para->r_clm[mm]=gsl_sf_log(2.5);
		}
	}                          /*LMean*/

	for (l=0;l<2*D->L;l++)          {D_para->tau_r_cl[l]=15;}                  /*Precision*/

	for (l=0;l<D->L;l++)          {D_para->r_o_l[l]=gsl_sf_log(2.5);}        /*LMean*/
	D_para->sigma_r_o=16;               /*Precision*/

	D_para->r_p=gsl_sf_log(2.5);       /*LMean*/

	/*nu*/
	for (l=0;l<D->L;l++)          {D_para->nu_l[l]=18;}                      /*LMean*/
	D_para->sigma_nu=0.0025;   /*Precision for lMean*/

	D_para->nu_p=18;   /*LMean*/
	/*P*/
	D_para->P=gsl_sf_log(0.0001);      /*LMean*/
	
	for (l=0;l<D->L;l++)          {D_para->gamma[l]=0;} 

	for (l=0;l<D->L;l++)          {D_para->omega[l]=0;}
	for (l=0;l<D->L;l++)          {D_para->delta[l]=1;}/*!*/  
 
	D_para->alpha[0]=gsl_sf_log(1);
	D_para->beta[0]=gsl_sf_log(1);
	D_para->alpha[1]=gsl_sf_log(1);
	D_para->beta[1]=gsl_sf_log(1);  
	D_para->sigma_gamma=1;
	D_para->sigma_omega=1;
	D_para->upsilon_c[0]=1; 
	D_para->upsilon_c[1]=1;       D_para->sigma_upsilon=1;
	D_para->upsilon_p=1;


return 0;
}

int fillpriors(struct_priors *D_priors)
{
	/*Priors*/
	/*K*/
	D_priors->sigma_K=1;               D_priors->phi_K=1/4^2;               /*Gamma  Shape; Scale */
	D_priors->eta_K_p=1;               D_priors->psi_K_o=1;             /*Gamma  Shape; Scale */
	/*r*/
	D_priors->sigma_r=1;               D_priors->phi_r=1/4^2;               /*Gamma  Shape; Scale */
	D_priors->eta_r_p=1;               D_priors->psi_r_o=1;             /*Gamma  Shape; Scale */
	/*nu*/
	D_priors->eta_nu_p=1;              D_priors->psi_nu=1;              /*Gamma  Shape; Scale */

	/*K*//*r*//*nu*//*P*/
	D_priors->K_mu=gsl_sf_log(0.2192928);      D_priors->eta_K_mu=1/3^2;      /*Normal  LMean; Precisions */
	D_priors->r_mu=gsl_sf_log(2.5);            D_priors->eta_r_mu=1/4^2;      /*Normal  LMean; Precisions */
	D_priors->nu_mu=gsl_sf_log(31);            D_priors->eta_nu_mu=1;     /*Normal  LMean; Precisions */
	D_priors->P_mu=gsl_sf_log(0.0001);         D_priors->eta_P_mu=1;   /*Normal  LMean; Precisions */
	/*data2.c*/       

	D_priors->alpha_mu=gsl_sf_log(1);          D_priors->eta_alpha=1/3^2;
	D_priors->beta_mu=gsl_sf_log(1);           D_priors->eta_beta=1/4^2;
	D_priors->p=0.1;    
	D_priors->eta_gamma=1;           	   D_priors->psi_gamma=1;
	D_priors->eta_omega=1;           	   D_priors->psi_omega=1;
	D_priors->eta_upsilon=1;	 	   D_priors->phi_upsilon=1;	    
	D_priors->upsilon_mu=1;			   D_priors->eta_upsilon_mu=1;


return 0;
}

