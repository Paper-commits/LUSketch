#设置交换机类型和拓扑
BMV2_SWITCH_EXE = simple_switch_grpc
TOPO = pod-topo/topology.json
#设置文件的目录，输出
BUILD_DIR = build
PCAP_DIR = pcaps
LOG_DIR = logs

#编译器，目标是bmv2
P4C = p4c-bm2-ss

# $@ 表示目标文件，basename取其前缀
P4C_ARGS += --p4runtime-files $(BUILD_DIR)/$(basename $@).p4.p4info.txt

#echo $(P4C_AGRS)
#运行脚本
RUN_SCRIPT = ../../utils/run_exercise.py

#默认拓扑
ifndef TOPO
TOPO = topology.json
endif

#源是所有的p4文件;变量替换语法，编译之后的文件名，后缀改为.json
source = $(wildcard *.p4)
compiled_json := $(source:.p4=.json)

#配置文件吧
ifndef DEFAULT_PROG
DEFAULT_PROG = $(wildcard *.p4)
endif
DEFAULT_JSON = $(BUILD_DIR)/$(DEFAULT_PROG:.p4=.json)

#使用编译出来的json作为配置文件，给交换机
ifndef NO_P4
run_args += -j $(DEFAULT_JSON)
endif

#将我们定义的交换机类型，覆盖默认类型
ifdef BMV2_SWITCH_EXE
run_args += -b $(BMV2_SWITCH_EXE)
endif

#启动的顺序：
#--创建目录--
#--编译p4文件--
#p4runtime-files API描述
#-o 目标文件
# 编译文件
#--运行脚本【拓扑】【交换机配置文件 交换机类型】--
all:run

run:build
	sudo python $(RUN_SCRIPT) -t $(TOPO) $(run_args)

stop:
	sudo mn -c

build: dirs $(compiled_json)

%.json: %.p4
	$(P4C) --p4v 16 $(P4C_ARGS) -o $(BUILD_DIR)/$@ $<

dirs:
	mkdir -p $(BUILD_DIR) $(PCAP_DIR) $(LOG_DIR)

clean: stop
	rm -f *.pcap
	rm -rf $(BUILD_DIR) $(PCAP_DIR) $(LOG_DIR)
