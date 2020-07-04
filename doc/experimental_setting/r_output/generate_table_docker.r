list.of.packages <- c("dplyr","xtable","magrittr")#,"dplyr","ggplot2","grid","ggthemes","gridExtra","lattice")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
#library(base)
library(xtable)
library(magrittr) # needs to be run every time you start R and want to use %>%
library(dplyr)
#library(ggplot2)
#library(gridExtra)
#library(lattice)
#require(scales)


experimental_composite <- read.csv(file='/tmp-results/unrealizable_data_composite.csv')
experimental_composite$liveness_count <- experimental_composite$guarantees_count + experimental_composite$assumptions_count
experimental_composite$reduction <- experimental_composite$minimization_transitions / experimental_composite$plant_transitions
summ_unreal <- experimental_composite %>%
  group_by(name) %>%
  summarize(diag = mean(diagnosis_time), steps = max(diagnosis_steps), liveness = max(liveness_count), plant_trans = max(plant_transitions),
            min_trans = max(minimization_transitions), red = mean(reduction))
realizable_composite = read.csv(file='/tmp-results/realizable_data_composite.csv')
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
table_contents <- xtable(composite_table, type = "latex", align = "r|l|rr|rr|rr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,3,0,0,0,0,4,0,4))
names(table_contents) <- c('Name', 'Diag. time(s)', "Diag. steps", '$|\\varphi_e + \\varphi_s|$', '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{E'}|/|\\Delta_{E}|$", "$|\\Delta_{C}|$", "$|\\Delta_{E'}|/|\\Delta_{C}|$")
addtorow <- list()
addtorow$pos <- list(-1)
addtorow$command <- paste0(paste0('\\hline & \\multicolumn{2}{c|}{Diagnosis}&\\multicolumn{2}{c|}{Plant ($E$)} & \\multicolumn{2}{c|}{Minimization ($v_{\\mathcal{U}}$)} & \\multicolumn{2}{c|}{Controller ($v_{\\mathcal{C}}$)}', collapse=''), '\\\\')

print(table_contents, file = "/tmp-results/experimental_data.tex", 
      add.to.row = addtorow, floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

