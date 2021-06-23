list.of.packages <- c("xtable","dplyr","magrittr","ggplot2","grid","ggthemes","gridExtra","lattice","base")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
library(base)
library(xtable)
library(magrittr) # needs to be run every time you start R and want to use %>%
library(dplyr)
library(ggplot2)
library(gridExtra)
library(lattice)
require(scales)


theme_Publication <- function(base_size=14, base_family="libertine") {
  library(grid)
  library(ggthemes)
  (theme_foundation(base_size=base_size)
    + theme(plot.title = element_text(face = "bold",
                                      size = rel(1.2), hjust = 0.5),
            text = element_text(),
            panel.background = element_rect(colour = NA),
            plot.background = element_rect(colour = NA),
            panel.border = element_rect(colour = NA),
            axis.title = element_text(face = "bold",size = rel(1)),
            axis.title.y = element_text(angle=90,vjust =2),
            axis.title.x = element_text(vjust = -0.2),
            axis.text = element_text(), 
            axis.line = element_line(colour="black"),
            axis.ticks = element_line(),
            panel.grid.major = element_line(colour="#f0f0f0"),
            panel.grid.minor = element_blank(),
            legend.key = element_rect(colour = NA),
            legend.position = "bottom",
            legend.direction = "horizontal",
            legend.key.size= unit(0.2, "cm"),
            legend.margin = unit(0, "cm"),
            legend.title = element_text(face="italic"),
            plot.margin=unit(c(10,5,5,5),"mm"),
            strip.background=element_rect(colour="#f0f0f0",fill="#f0f0f0"),
            strip.text = element_text(face="bold")
    ))
  
}

scale_fill_Publication <- function(...){
  library(scales)
  discrete_scale("fill","Publication",manual_pal(values = c("#386cb0","#fdb462","#7fc97f","#ef3b2c","#662506","#a6cee3","#fb9a99","#984ea3","#ffff33")), ...)
  
}

scale_colour_Publication <- function(...){
  library(scales)
  discrete_scale("colour","Publication",manual_pal(values = c("#386cb0","#fdb462","#7fc97f","#ef3b2c","#662506","#a6cee3","#fb9a99","#984ea3","#ffff33")), ...)
  
}

experimental_composite <- read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/unrealizable_data_composite.csv')
experimental_composite$liveness_count <- experimental_composite$guarantees_count + experimental_composite$assumptions_count
experimental_composite$reduction <- experimental_composite$minimization_transitions / experimental_composite$plant_transitions
summ_unreal <- experimental_composite %>%
  group_by(name) %>%
  summarize(diag = mean(diagnosis_time), steps = max(diagnosis_steps), liveness = max(liveness_count), plant_trans = max(plant_transitions),
            min_trans = max(minimization_transitions), red = mean(reduction))
realizable_composite = read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/realizable_data_composite.csv')
summ_real <- realizable_composite %>%
  group_by(name) %>%
  summarize(plant_trans = max(plant_transitions),  min_trans = max(minimization_transitions))

lift <- subset(summ_unreal, grepl("Lift\\.Controller\\.\\d\\.\\(missing", name))
lift_removed <- subset(summ_unreal, grepl("Lift\\.Controller\\.\\d\\.\\(removed", name))
genbuf_missing <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(missing", name))
genbuf_removed <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(removed", name))
ahb_missing <- subset(summ_unreal, grepl("AHB\\.Scheduler\\.\\d\\.\\(missing", name))
ahb_removed <- subset(summ_unreal, grepl("AHB\\.Scheduler\\.\\d\\.\\(removed", name))
collector_missing <- subset(summ_unreal, grepl("Collector\\.\\d\\.\\(missing", name))
collector_removed <- subset(summ_unreal, grepl("Collector\\.\\d\\.\\(removed", name))
robot_samples <- subset(summ_unreal, grepl("Robot\\.\\d*\\.\\(missing", name))
robot_removed <- subset(summ_unreal, grepl("Robot\\.\\d*\\.\\(removed", name))

lift_real <- subset(summ_real, grepl("Lift", name))
genbuf_real <- subset(summ_real, grepl("Genbuf", name))
ahb_real <- subset(summ_real, grepl("AHB", name))
collector_real <- subset(summ_real, grepl("Collector", name))
robot_real <- subset(summ_real, grepl("Robot", name))

lift$ctrl_transitions <- lift_real$min_trans
lift$reduction_ctrl <- lift$plant_trans / lift_real$min_trans
lift_removed$ctrl_transitions <- lift_real$min_trans
lift_removed$reduction_ctrl <- lift_removed$plant_trans / lift_real$min_trans
genbuf_missing$ctrl_transitions <- genbuf_real$min_trans
genbuf_missing$reduction_ctrl <- genbuf_missing$plant_trans / genbuf_real$min_trans
#removed env is missing the last value
genbuf_real_b <- genbuf_real[1:(nrow(genbuf_real)-1),]
genbuf_removed$ctrl_transitions <- genbuf_real_b$min_trans
genbuf_removed$reduction_ctrl <- genbuf_removed$min_trans / genbuf_real_b$min_trans
#genbuf_removed$ctrl_transitions <- genbuf_real$min_trans
#genbuf_removed$reduction_ctrl <- genbuf_removed$min_trans / genbuf_real$min_trans

ahb_missing$ctrl_transitions <- ahb_real$min_trans
ahb_missing$reduction_ctrl <- ahb_missing$min_trans / ahb_real$min_trans
ahb_removed$ctrl_transitions <- ahb_real$min_trans
ahb_removed$reduction_ctrl <- ahb_removed$min_trans / ahb_real$min_trans

collector_missing$ctrl_transitions <- collector_real$min_trans
collector_missing$reduction_ctrl <- collector_missing$min_trans / collector_real$min_trans
collector_removed$ctrl_transitions <- collector_real$min_trans
collector_removed$reduction_ctrl <- collector_removed$min_trans / collector_real$min_trans
robot_samples$ctrl_transitions <- robot_real$min_trans
robot_samples$reduction_ctrl <- robot_samples$min_trans / robot_real$min_trans
robot_removed$ctrl_transitions <- robot_real$min_trans
robot_removed$reduction_ctrl <- robot_removed$min_trans / robot_real$min_trans

composite_table <- rbind(lift,lift_removed,collector_missing, collector_removed,robot_samples, robot_removed,genbuf_missing, genbuf_removed,ahb_missing, ahb_removed)
composite_table$name <- gsub("\\.", " ", composite_table$name)
table_contents <- xtable(composite_table, type = "latex", align = "r|l|rr|rr|rr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,3,0,0,0,0,4,0,4))
table_contents$diag <- with(table_contents, ifelse(diag < 1, '$<$1', round(diag)))
table_contents$red <- ifelse(table_contents$red < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$red * 100), 4), nsmall = 2), "\\%"))
table_contents$reduction_ctrl <- ifelse(table_contents$reduction_ctrl < 0.00001, "$<1e^{-5}$ \\%",paste(format(round((table_contents$reduction_ctrl * 100), 4), nsmall = 2), "\\%"))
names(table_contents) <- c('Name', 'Diag. time(s)', "Diag. steps", '$|\\varphi_e + \\varphi_s|$', '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{E'}|/|\\Delta_{E}|$", "$|\\Delta_{C}|$", "$|\\Delta_{E'}|/|\\Delta_{C}|$")
addtorow <- list()
addtorow$pos <- list(-1)
addtorow$command <- paste0(paste0('\\hline & \\multicolumn{2}{c|}{Diagnosis}&\\multicolumn{2}{c|}{Plant ($E$)} & \\multicolumn{2}{c|}{Minimization ($v_{\\mathcal{U}}$)} & \\multicolumn{2}{c|}{Controller ($v_{\\mathcal{C}}$)}', collapse=''), '\\\\')

print(table_contents, file = "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/experimental_data.tex", 
      add.to.row = addtorow, floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

median(composite_table$red)
length(composite_table$red)

#sive vs diagnosis time
time_summ <- experimental_composite %>% group_by(plant_transitions) %>%
  summarize(ymin = min(diagnosis_time),
            ymax = max(diagnosis_time),
            ymean = mean(diagnosis_time))

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/size_vs_diag_time.ps")
ggplot(time_summ, aes(x=plant_transitions, y=ymean)) +
  labs(title="Size vs diagnosis time") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10(labels=comma) +
  scale_x_log10(labels=comma) +	
  geom_point(color='red') +	
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()
#+geom_errorbar(aes(ymin = ymin, ymax = ymax), width=.05,position=position_dodge(0.05))
dev.off()
model <- lm(time_summ$plant_transitions ~ time_summ$ymean)
summary(model)
boxplot(model[['residuals']],main='Boxplot: Residuals',ylab='residual value')
cor.test(time_summ$plant_transitions, time_summ$ymean,  method="kendall")

#plant controllability vs minimization amount
no_robot_safety_composite <- subset(experimental_composite, !(grepl("Robot\\.\\d*\\.", name)))
c_coeff = no_robot_safety_composite$minimization_controllable_transitions / no_robot_safety_composite$plant_transitions	
m_coeff = no_robot_safety_composite$minimization_transitions / no_robot_safety_composite$plant_transitions	

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/min_ctrl_vs_min_pct.ps")
ggplot(no_robot_safety_composite, aes(x=c_coeff, y=m_coeff)) +
  labs(title="Minimization controllability vs minimization amount") +
  xlab("Minimization controllability") +
  ylab("Reduction") +
  scale_y_log10(labels=comma) +
  scale_x_log10(labels=comma) +	
  geom_point(color='red') +
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication() 
dev.off()
cor.test(c_coeff, m_coeff,  method="kendall")

df <- read.csv(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/steps/genbuf_3_sndrs_no_automaton_missing_assumption.csv")
postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/genbuf_3_time_plot.ps")
time_plot <- ggplot(df, aes(x=step, y=time)) +
  geom_line(linetype="dashed", color="blue", size=1.2)+
  geom_point(color="red", size=3) + 
  labs(title="GenBuf 3 senders time progression") +
  xlab("Steps") +
  ylab("Time (s)")  +
  scale_y_continuous(label=comma) + scale_x_continuous(label=comma) + 
  scale_colour_Publication()+ theme_Publication() 
print(time_plot)
dev.off()
postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/genbuf_3_size_plot.ps")
size_plot <- ggplot(df, aes(x=step, y=size)) +
  geom_line(linetype="dashed", color="blue", size=1.2)+
  geom_point(color="red", size=3) + 
  labs(title="GenBuf 3 senders size progression") +
  xlab("Steps") +
  ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
  scale_y_continuous(label=comma) + scale_x_continuous(label=comma) + 
  scale_colour_Publication()+ theme_Publication() 
print(size_plot)  
dev.off()


#size_diag = lm(c_coeff ~ m_coeff, data= experimental_composite)
#summary(size_diag)
#coef(size_diag)["diagnosis_time"] + 2* summary(size_diag)$coef["diagnosis_time", "Std. Error"]

#rows, cols
sizediag_g_ass <- ggplot(genbuf_missing, aes(x=plant_trans, y=diag)) +
  labs(title="Genbuf.ass.") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

sizediag_safety <- ggplot(genbuf_removed, aes(x=plant_trans, y=diag)) +
  labs(title="Genbuf.safety") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

sizediag_collector <- ggplot(collector_missing, aes(x=plant_trans, y=diag)) +
  labs(title="Collector") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

sizediag_exploration <- ggplot(robot_samples, aes(x=plant_trans, y=diag)) +
  labs(title="Robot") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

plantminsize_g_ass <- ggplot(genbuf_missing, aes(x=plant_trans, y=min_trans)) +
  labs(title="Genbuf.ass.") +
  xlab(expression(paste("|",Delta[E],"|"))) +
  ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

plantminsize_safety <- ggplot(genbuf_removed, aes(x=plant_trans, y=min_trans)) +
  labs(title="Genbuf.safety") +
  xlab(expression(paste("|",Delta[E],"|"))) +
  ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm')		 +
  scale_colour_Publication()+ theme_Publication()

plantminsize_collector <- ggplot(collector_missing, aes(x=plant_trans, y=min_trans)) +
  labs(title="Collector") +
  xlab(expression(paste("|",Delta[E],"|"))) +
  ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red')  +
  scale_colour_Publication()+ theme_Publication()

#+	geom_smooth(method='lm')	
plantminsize_exploration <- ggplot(robot_samples, aes(x=plant_trans, y=min_trans)) +
  labs(title="Robot") +
  xlab(expression(paste("|",Delta[E],"|"))) +
  ylab(expression(paste("|",Delta[paste("E","'")],"|"))) +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +	
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

c_coeff_ga = genbuf_missing$ctrl_transitions / genbuf_missing$plant_trans
m_coeff_ga = genbuf_missing$min_trans / genbuf_missing$plant_trans	
control_g_ass <- ggplot(genbuf_missing, aes(x=c_coeff_ga, y=m_coeff_ga)) +
  labs(title="Genbuf.ass") +
  xlab("Controllability") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

c_coeff_gs = genbuf_removed$ctrl_transitions / genbuf_removed$plant_trans
m_coeff_gs = genbuf_removed$min_trans / genbuf_removed$plant_trans	
control_safety <- ggplot(genbuf_removed, aes(x=c_coeff_gs, y=m_coeff_gs)) +
  labs(title="Genbuf.safety") +
  xlab("Controllability") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

c_coeff_c = collector_missing$ctrl_transitions / collector_missing$plant_trans	
m_coeff_c = collector_missing$min_trans / collector_missing$plant_trans	
control_collector <- ggplot(collector_missing, aes(x=c_coeff_c, y=m_coeff_c)) +
  labs(title="Collector") +
  xlab("Controllability") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red')  +
  scale_colour_Publication()+ theme_Publication()

# +	geom_smooth(method='lm')
c_coeff_r = robot_samples$ctrl_transitions / robot_samples$plant_trans	
m_coeff_r = robot_samples$min_trans / robot_samples$plant_trans	
control_robot <- ggplot(robot_samples, aes(x=c_coeff_r, y=m_coeff_r)) +
  labs(title="Robot") +
  xlab("Controllability") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

m_c_coeff_ga = genbuf_missing$ctrl_transitions / genbuf_missing$plant_trans	
m_m_coeff_ga = genbuf_missing$min_trans / genbuf_missing$plant_trans	
m_control_g_ass <- ggplot(genbuf_missing, aes(x=m_c_coeff_ga, y=m_m_coeff_ga)) +
  labs(title="Genbuf.ass") +
  xlab("Min. Cont.") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

m_c_coeff_gs = genbuf_removed$ctrl_transitions / genbuf_removed$plant_trans	
m_m_coeff_gs = genbuf_removed$min_trans / genbuf_removed$plant_trans	
m_control_safety <- ggplot(genbuf_removed, aes(x=m_c_coeff_gs, y=m_m_coeff_gs)) +
  labs(title="Genbuf.safety") +
  xlab("Min. Cont.") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

m_c_coeff_c = collector_missing$ctrl_transitions / collector_missing$plant_trans	
m_m_coeff_c = collector_missing$min_trans / collector_missing$plant_trans	
m_control_collector <- ggplot(collector_missing, aes(x=m_c_coeff_c, y=m_m_coeff_c)) +
  labs(title="Collector") +
  xlab("Min. Cont.") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()

m_c_coeff_r = robot_samples$ctrl_transitions / robot_samples$plant_trans	
m_m_coeff_r = robot_samples$min_trans / robot_samples$plant_trans	
m_control_robot <- ggplot(robot_samples, aes(x=m_c_coeff_r, y=m_m_coeff_r)) +
  labs(title="Robot") +
  xlab("Min. Cont.") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/compared_graphs.ps")
grid.arrange(sizediag_g_ass, sizediag_safety, sizediag_collector, sizediag_exploration, plantminsize_g_ass, plantminsize_safety, plantminsize_collector, plantminsize_exploration, control_g_ass, control_safety, control_collector, control_robot, m_control_g_ass, m_control_safety, m_control_collector, m_control_robot, ncol=4)
dev.off()


seq_composite <- read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/seq_data_composite.csv')
seq_composite_game <- dplyr::filter(seq_composite, !grepl("intrlvd",filename) & !grepl("seq",filename))
seq_composite_intrlvd <- dplyr::filter(seq_composite, grepl("intrlvd",filename))
seq_composite_seq <- dplyr::filter(seq_composite, grepl("seq",filename))
seq_composite_game_intrlvd_conform <- seq_composite_game[-c(21,22,23,27),]

seq_composite_game$states_ratio <- seq_composite_game$states_count / seq_composite_seq$states_count
seq_composite_game$transitions_ratio <- seq_composite_game$transitions_count / seq_composite_seq$transitions_count;
seq_composite_game_intrlvd_conform$states_ratio <- seq_composite_game_intrlvd_conform$states_count / seq_composite_intrlvd$states_count
seq_composite_game_intrlvd_conform$transitions_ratio <- seq_composite_game_intrlvd_conform$transitions_count / seq_composite_intrlvd$transitions_count;

seq_composite_game_no_robot <- dplyr::filter(seq_composite_game, !grepl("robot",filename))
seq_composite_game_robot <- dplyr::filter(seq_composite_game, !grepl("robot",filename))
seq_composite_game_intrlvd_conform_no_robot <- dplyr::filter(seq_composite_game_intrlvd_conform, !grepl("robot",filename))
seq_composite_game_intrlvd_conform_robot <- dplyr::filter(seq_composite_game_intrlvd_conform, !grepl("robot",filename))

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/seq_state_seq.ps")
m_seq_state_seq <- ggplot(seq_composite_game, aes(x=states_count, y=states_ratio)) +
  labs(title="Seq. states reduction ratio") +
  xlab("|S_E|") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  #geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()
print(m_seq_state_seq)
dev.off()

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/seq_trans_seq.ps")
m_seq_trans_seq <- ggplot(seq_composite_game, aes(x=transitions_count, y=transitions_ratio)) +
  labs(title="Seq. transition reduction ratio") +
  xlab("|Delta_E|") +
  ylab("Reduction") +
  scale_y_log10() +
  scale_x_log10() +	
  geom_point(color='red') +
  #geom_smooth(method='lm')	 +
  scale_colour_Publication()+ theme_Publication()
print(m_seq_trans_seq)
dev.off()

