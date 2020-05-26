install.packages("ggplot2")
library(ggplot2)
install.packages("jsonlite")
library(jsonlite)

#df <- data.frame(one=c(1,4,5), three= c('a','b','c'))
#df$two <- list(c(2,3),c(1,1), c(5,3))
#write.csv(df, file="/home/mariano/code/henos-automata/doc/experimental_setting/r_output/listTest.Rdata")
#toJSON(df, pretty=T, auto_unbox = T)

experimental_composite <- read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/general_data_composite.csv')


lines(experimental_composite$plant_transitions, experimental_composite$diagnosis_time, type="p")
#sive vs diagnosis time
ggplot(experimental_composite, aes(x=plant_transitions, y=diagnosis_time)) +
	labs(title="Size vs diagnosis time") +
	xlab(expression(paste("|",Delta["E"],"|"))) +
	ylab("Diagnosis time (s)") +
	scale_y_log10() +
	scale_x_log10() +	
	geom_point(color='red') +	
	geom_smooth(method='lm')
#plant size vs minimization time	
ggplot(experimental_composite, aes(x=plant_transitions, y=minimization_transitions)) +
	labs(title="Plant size vs minimization size") +
	xlab(expression(paste("|",Delta[E],"|"))) +
	ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
	scale_y_log10() +
	scale_x_log10() +	
	geom_point(color='red') +	
	geom_smooth(method='lm')
#plant size vs minimization time w labels
ggplot(experimental_composite, aes(x=plant_transitions, y=minimization_transitions, label=name)) +
	labs(title="Plant size vs minimization size") +
	xlab(expression(paste("|",Delta[E],"|"))) +
	ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
	scale_y_log10() +
	scale_x_log10() +	
	geom_point(color='red') +	
	geom_text(aes(label=as.character(name)),hjust=0,vjust=0) +
	geom_smooth(method='lm')
#plant controllability vs minimization amount
c_coeff = experimental_composite$plant_controllable_transitions / experimental_composite$plant_transitions	
m_coeff = experimental_composite$minimization_transitions / experimental_composite$plant_transitions	
ggplot(experimental_composite, aes(x=c_coeff, y=m_coeff)) +
	labs(title="Plant controllability vs minimization amount") +
	xlab("Controllability") +
	ylab("Reduction") +
	scale_y_log10() +
	scale_x_log10() +	
	geom_point(color='red') +
	geom_smooth(method='lm')		
#plant controllability vs minimization amount w labels
c_coeff = experimental_composite$plant_controllable_transitions / experimental_composite$plant_transitions	
m_coeff = experimental_composite$minimization_transitions / experimental_composite$plant_transitions	
ggplot(experimental_composite, aes(x=c_coeff, y=m_coeff, label=name)) +
	labs(title="Plant controllability vs minimization amount") +
	xlab("Controllability") +
	ylab("Reduction") +
	scale_y_log10() +
	scale_x_log10() +	
	geom_point(color='red') +
	geom_text(aes(label=as.character(name)),hjust=0,vjust=0) +
	geom_smooth(method='lm')	
