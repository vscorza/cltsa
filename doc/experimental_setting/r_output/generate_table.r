list.of.packages <- c("xtable","dplyr","magrittr")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)
library(xtable)
library(magrittr) # needs to be run every time you start R and want to use %>%
library(dplyr)
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

genbuf_missing <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(missing", name))
genbuf_removed <- subset(summ_unreal, grepl("Genbuf\\.\\d\\.\\(removed", name))
collector_missing <- subset(summ_unreal, grepl("Collector\\.\\d\\.\\(missing", name))
robot_samples <- subset(summ_unreal, grepl("Robot", name))

genbuf_real <- subset(summ_real, grepl("Genbuf", name))
collector_real <- subset(summ_real, grepl("Collector", name))
robot_real <- subset(summ_real, grepl("Robot", name))

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
composite_table <- rbind(collector_missing, robot_samples, genbuf_missing, genbuf_removed)
composite_table$name <- gsub("\\.", " ", composite_table$name)
table_contents <- xtable(composite_table, type = "latex", align = "r|l|rrr|r|rr|rr|",caption="Quantitative results for minimization plants"
                         ,digits=c(0,0,0,3,0,0,0,4,0,4))
names(table_contents) <- c('Name', "$|\\varphi_e + \\varphi_s|$", 'Diag. time(s)', "Diag. steps", '$|\\Delta_E|$', "$|\\Delta_{E'}|$", "$|\\Delta_{E'}|/|\\Delta_{E}|$", "$|\\Delta_{C}|$", "$|\\Delta_{E'}|/|\\Delta_{C}|$")
print(table_contents, file = "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/experimental_data.tex", floating= FALSE, include.rownames=FALSE, sanitize.text.function=function(x){x})

