list.of.packages <- c("xtable","dplyr","magrittr","ggplot2","grid","ggthemes","gridExtra","lattice")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
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
  summarize(liveness = max(liveness_count), diag = mean(diagnosis_time), steps = max(diagnosis_steps), plant_trans = max(plant_transitions),
            min_trans = max(minimization_transitions), red = mean(reduction))
realizable_composite = read.csv(file='/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/realizable_data_composite.csv')
summ_real <- realizable_composite %>%
  group_by(name) %>%
  summarize(plant_trans = max(plant_transitions),  min_trans = max(minimization_transitions))

lift <- subset(summ_unreal, grepl("Lift\\.Controller\\.\\d\\.\\(missing", name))
genbuf_missing <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(missing", name))
genbuf_removed <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(removed", name))
collector_missing <- subset(summ_unreal, grepl("Collector\\.\\d\\.\\(missing", name))
robot_samples <- subset(summ_unreal, grepl("Robot\\.\\d*\\.\\(missing", name))

lift_real <- subset(summ_real, grepl("Lift", name))
genbuf_real <- subset(summ_real, grepl("Genbuf", name))
collector_real <- subset(summ_real, grepl("Collector", name))
robot_real <- subset(summ_real, grepl("Robot", name))

lift$ctrl_transitions <- lift_real$min_trans
lift$reduction_ctrl <- lift$plant_trans / lift_real$min_trans
genbuf_missing$ctrl_transitions <- genbuf_real$min_trans
genbuf_missing$reduction_ctrl <- genbuf_missing$plant_trans / genbuf_real$min_trans
#removed env is missing the last value
genbuf_real_b <- genbuf_real[1:(nrow(genbuf_real)-1),]
genbuf_removed$ctrl_transitions <- genbuf_real_b$min_trans
genbuf_removed$reduction_ctrl <- genbuf_removed$min_trans / genbuf_real_b$min_trans
collector_missing$ctrl_transitions <- collector_real$min_trans
collector_missing$reduction_ctrl <- collector_missing$min_trans / collector_real$min_trans
robot_samples$ctrl_transitions <- robot_real$min_trans
robot_samples$reduction_ctrl <- robot_samples$min_trans / robot_real$min_trans
composite_table <- rbind(lift,collector_missing, robot_samples, genbuf_missing, genbuf_removed)
composite_table$name <- gsub("\\.", " ", composite_table$name)
table_contents <- xtable(composite_table, type = "latex", align = "r|l|rrr|r|rr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,0,3,0,0,0,4,0,4))
names(table_contents) <- c('Name', "$|\\varphi_e + \\varphi_s|$", 'Diag. time(s)', "Diag. steps", '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{E'}|/|\\Delta_{E}|$", "$|\\Delta_{C}|$", "$|\\Delta_{E'}|/|\\Delta_{C}|$")
print(table_contents, file = "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/experimental_data.tex", floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

#sive vs diagnosis time
postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/size_vs_diag_time.ps")
ggplot(experimental_composite, aes(x=plant_transitions, y=diagnosis_time)) +
  labs(title="Size vs diagnosis time") +
  xlab(expression(paste("|",Delta["E"],"|"))) +
  ylab("Diagnosis time (s)") +
  scale_y_log10(labels=comma) +
  scale_x_log10(labels=comma) +	
  geom_point(color='red') +	
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication()
dev.off()

#plant controllability vs minimization amount
c_coeff = experimental_composite$minimization_controllable_transitions / experimental_composite$plant_transitions	
m_coeff = experimental_composite$minimization_transitions / experimental_composite$plant_transitions	

postscript(file="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/min_ctrl_vs_min_pct.ps")
ggplot(experimental_composite, aes(x=c_coeff, y=m_coeff)) +
  labs(title="Minimization controllability vs minimization amount") +
  xlab("Minimization controllability") +
  ylab("Reduction") +
  scale_y_log10(labels=comma) +
  scale_x_log10(labels=comma) +	
  geom_point(color='red') +
  geom_smooth(method='lm') +
  scale_colour_Publication()+ theme_Publication() 
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