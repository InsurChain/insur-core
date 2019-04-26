package model

//Limits Limits
type Limits struct {
	MaxConn uint32		`json:"max_connection"`
	Speed   float64		`json:"speed"`
}

//Hardware Hardware
type Hardware struct {
	BandWidth	float64 `json:"bandwidth"`
	CPU			interface{} `json:"cpu"`
	GPU			interface{} `json:"GPU"`
	Disk		interface{} `json:"disk"`
}

//NodeInfo NodeInfo
type NodeInfo struct {
	NodeID        string   `json:"node_id"`
	Alias         string   `json:"alias"`
	VPSAddr       string   `json:"vps_addr"`
	CNATAddr      string   `json:"c_nat_addr"`
	DNATAddr      string   `json:"d_net_addr"`
	Region        string   `json:"region"`
	WorkStatus    int   `json:"work_status"`
	Roler         int      `json:"roler"`
	ControlPort   int      `json:"control_port"`
	DataTunnPort  int      `json:"data_tunn_port"`
	FreshInterval int64    `json:"fresh_interval"`
	ProvideList   []string `json:"provide_list"`
}

//BasicNode BasicNode
type BasicNode struct {
	NodeInfo	   NodeInfo	`json:"node_info"`	
	Hardware	   Hardware	`json:"hardware"`
	Limits		   Limits	`json:"limits"`
}

//ModelName return model name
func (b *BasicNode) ModelName() string {
	return "basic_node"
}


   
