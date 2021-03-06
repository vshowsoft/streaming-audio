/*
 samclient~ - SAM client for streaming audio to SAM server
 
 Developed by Zachary Seldess, Sonic Arts R&D, Calit2, UC San Diego
 */

#define MSP //<-- Comment out for Pd build

#ifndef SAC_NO_JACK
#define SAC_NO_JACK
#endif

#ifdef MSP
#include "ext.h"        // standard Max include, always required
#include "ext_obex.h"   // required for new style Max object
#include "z_dsp.h"      // main header for all Max DSP objects
#else
extern "C"
{
    #include "m_pd.h"   // standard Pd include, always require
}
#endif

#ifdef error
#undef error // Qt has an error() function, so we need to undo #define in ext.h
#endif

#include "sam_client.h" // sam client header


#ifdef MSP
/*************************** Max stuff ************************/
////////////////////////// object struct
typedef struct _samclient 
{
    t_pxobject m_obj;   // the object itself (must be first, Max)
    t_symbol *name;     // SAM client name
    t_symbol *ip;       // SAM ip
    int port;           // SAM port
    int channels;       // number of channels to stream
    int type;           // renderer type for SAM client
    int preset;         // renderer preset for SAM client
    int x_pos;          // initial x position coordinate
    int y_pos;          // initial y position coordinate
    int width;          // initial width for SAM stream
    int height;         // initial height for SAM stream
    int depth;          // initial depth for SAM stream
    int pqsize;         // initial packet queue size
    int vecsize;        // MSP signal vector size
    int sam_vecsize;    // SAM signal vector size
    float samprate;     // MSP sampling rate
    float sam_samprate; // SAM sampling rate
    float **sigvec;     // 2D array to hold n channels of current signal vector of input samples (used in perform routine)
    int offset;         // sample offset in sigvec 2D array
    int sac_state;      // flag for SAC connection
    void *m_outlet1;    // pointer to left outlet (for Max)
    sam::StreamingAudioClient *sac = NULL;
} t_samclient;

///////////////////////// function prototypes
void *samclient_new(t_symbol *s, int argc, t_atom *argv);
void samclient_free(t_samclient *x);
void samclient_assist(t_samclient *x, void *b, long m, long a, char *s);
void sac_connect(t_samclient *x);
void sac_disconnect(t_samclient *x);
void sac_create(t_samclient *x);
void sac_destroy(t_samclient *x);
void samclient_dspstate(t_samclient *x, long state);
void samclient_dsp(t_samclient *x, t_signal **sp, short *count);
void samclient_dsp64(t_samclient *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
t_int *samclient_perform(t_int *w);
void samclient_perform64(t_samclient *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
t_max_err samclient_port_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_port_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_type_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_type_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_preset_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_preset_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_x_pos_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_x_pos_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_y_pos_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_y_pos_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_width_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_width_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_height_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_height_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_depth_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_depth_set(t_samclient *x, void *attr, long argc, t_atom *argv);
t_max_err samclient_pqsize_get(t_samclient *x, void *attr, long *argc, t_atom **argv);
t_max_err samclient_pqsize_set(t_samclient *x, void *attr, long argc, t_atom *argv);

#else
/*************************** Pd stuff ************************/
extern "C"
{
    ////////////////////////// object struct
    typedef struct _samclient
    {
        t_object m_obj;     // the object itself (must be first, Pd)
        t_float x_f;        // for main signal inlet
        t_symbol *name;     // SAM client name
        t_symbol *ip;       // SAM ip
        int port;           // SAM port
        int channels;       // number of channels to stream
        int type;           // renderer type for SAM client
        int preset;         // renderer preset for SAM client
        int x_pos;          // initial x position coordinate
        int y_pos;          // initial y position coordinate
        int width;          // initial width for SAM stream
        int height;         // initial height for SAM stream
        int depth;          // initial depth for SAM stream
        int pqsize;         // initial packet queue size
        int vecsize;        // MSP signal vector size
        int sam_vecsize;    // SAM signal vector size
        float samprate;     // MSP sampling rate
        float sam_samprate; // SAM sampling rate
        float **sigvec;     // 2D array to hold n channels of current signal vector of input samples (used in perform routine)
        int offset;         // sample offset in sigvec 2D array
        int sac_state;      // flag for SAC connection
        void *m_outlet1;    // pointer to left outlet (for Max)
        sam::StreamingAudioClient *sac = NULL;
    } t_samclient;

    ///////////////////////// function prototypes
    void *samclient_new(t_symbol *s, int argc, t_atom *argv);
    void samclient_free(t_samclient *x);
    void sac_connect(t_samclient *x);
    void sac_disconnect(t_samclient *x);
    void sac_create(t_samclient *x);
    void sac_destroy(t_samclient *x);
    void samclient_name(t_samclient *x, t_symbol *s);
    void samclient_ip(t_samclient *x, t_symbol *s);
    void samclient_port(t_samclient *x, t_floatarg f);
    void samclient_type(t_samclient *x, t_floatarg f);
    void samclient_preset(t_samclient *x, t_floatarg f);
    void samclient_x_pos(t_samclient *x, t_floatarg f);
    void samclient_y_pos(t_samclient *x, t_floatarg f);
    void samclient_width(t_samclient *x, t_floatarg f);
    void samclient_height(t_samclient *x, t_floatarg f);
    void samclient_depth(t_samclient *x, t_floatarg f);
    void samclient_dsp(t_samclient *x, t_signal **sp);
    void samclient_pqsize(t_samclient *x, t_floatarg f);
    t_int *samclient_perform(t_int *w);
}
#endif

//////////////////////// global class pointer variable
static t_class *samclient_class;

#ifdef MSP
/*************************** Max stuff ************************/
int C74_EXPORT main(int argc, char **argv)
{       
    t_class *c;
    c = class_new("samclient~", (method)samclient_new, (method)samclient_free, (long)sizeof(t_samclient), 0L, A_GIMME, 0);
    
    class_addmethod(c, (method)samclient_dspstate, "dspstate", A_CANT, 0);
	class_addmethod(c, (method)samclient_dsp, "dsp", A_CANT, 0); // Old 32-bit MSP dsp chain compilation for Max 5 and earlier
	class_addmethod(c, (method)samclient_dsp64, "dsp64", A_CANT, 0); // New 64-bit MSP dsp chain compilation for Max 6 
    class_addmethod(c, (method)stdinletinfo, "inletinfo", A_CANT, 0); // all right inlets are cold
    class_addmethod(c, (method)samclient_assist, "assist", A_CANT, 0); // you CAN'T call this from the patcher
    class_addmethod(c, (method)sac_connect, "connect", 0);
    class_addmethod(c, (method)sac_disconnect, "disconnect", 0);

    CLASS_ATTR_SYM(c, "name", 0, t_samclient, name); // define "name" attribute
	CLASS_ATTR_SYM(c, "ip", 0, t_samclient, ip); // define "ip" attribute
	CLASS_ATTR_LONG(c, "port", 0, t_samclient, port); // define "port" attribute
    CLASS_ATTR_LONG(c, "type", 0, t_samclient, type); // define "type" attribute
    CLASS_ATTR_LONG(c, "preset", 0, t_samclient, preset); // define "preset" attribute
    CLASS_ATTR_LONG(c, "x_pos", 0, t_samclient, x_pos); // define "x_pos" attribute
    CLASS_ATTR_LONG(c, "y_pos", 0, t_samclient, y_pos); // define "y_pos" attribute
    CLASS_ATTR_LONG(c, "width", 0, t_samclient, width); // define "width" attribute
    CLASS_ATTR_LONG(c, "height", 0, t_samclient, height); // define "height" attribute
    CLASS_ATTR_LONG(c, "depth", 0, t_samclient, depth); // define "depth" attribute
    CLASS_ATTR_LONG(c, "pqsize", 0, t_samclient, pqsize); // define "pqsize" attribute
    
    // override default accessors
    CLASS_ATTR_ACCESSORS(c, "port", (method)samclient_port_get, (method)samclient_port_set);
    CLASS_ATTR_ACCESSORS(c, "type", (method)samclient_type_get, (method)samclient_type_set);
    CLASS_ATTR_ACCESSORS(c, "preset", (method)samclient_preset_get, (method)samclient_preset_set);
    CLASS_ATTR_ACCESSORS(c, "x_pos", (method)samclient_x_pos_get, (method)samclient_x_pos_set);
    CLASS_ATTR_ACCESSORS(c, "y_pos", (method)samclient_y_pos_get, (method)samclient_y_pos_set);
    CLASS_ATTR_ACCESSORS(c, "width", (method)samclient_width_get, (method)samclient_width_set);
    CLASS_ATTR_ACCESSORS(c, "height", (method)samclient_height_get, (method)samclient_height_set);
    CLASS_ATTR_ACCESSORS(c, "depth", (method)samclient_depth_get, (method)samclient_depth_set);
    CLASS_ATTR_ACCESSORS(c, "pqsize", (method)samclient_pqsize_get, (method)samclient_pqsize_set);
    
    class_dspinit(c); // new style object version of dsp_initclass();
    class_register(CLASS_BOX, c);
    samclient_class = c;
        
    return 0;
}

void *samclient_new(t_symbol *s, int argc, t_atom *argv)
{
    t_samclient *x = NULL;
   
    if ((x = (t_samclient *)object_alloc(samclient_class))) {
        // You MUST set the below flag if you want memory addresses
        // of outlets to be different from their corresponding inlets! 
        //x->m_obj.z_misc |= Z_NO_INPLACE;
        x->m_outlet1 = outlet_new((t_object *)x, NULL); // left outlet
      
        int i, attr;
        
        // initialize class member defaults
        x->name = gensym("samclient");
        x->ip = gensym("127.0.0.1");
        x->port = 7770;
        x->channels = 1;
        x->type = 0;
        x->preset = 0;
        x->x_pos = 0;
        x->y_pos = 0;
        x->width = 0;
        x->height = 0;
        x->depth = 0;
        x->pqsize = -1;
        x->vecsize = sys_getblksize();
        x->sam_vecsize = x->vecsize;
        x->samprate = sys_getsr();
        x->sam_samprate = x->samprate;
        x->offset = 0;
        x->sac_state = 0;

        // get number of args
        attr = attr_args_offset(argc, argv);
        
        // update x->channels, if argument provided
        if (attr >= 1 && (atom_gettype(argv) == A_FLOAT || atom_gettype(argv) == A_LONG)) {
            if (atom_getlong(argv) >= 1) {
                x->channels = atom_getlong(argv);
            }
        }
        
        // parse attributes, return number of args remaining
        attr_args_process(x, argc, argv);
        
        // MSP inlets: arg is # of inlets and is REQUIRED (use 0 if no MSP inlets)
        dsp_setup((t_pxobject *)x, x->channels);

        // allocate memory for arrays, initialize struct members
        x->sigvec = (float **) sysmem_newptrclear(x->channels * sizeof(float *));
        for (i = 0; i < x->channels; i++) {
            x->sigvec[i] = (float *) sysmem_newptrclear(x->sam_vecsize * sizeof(float));
        }
    }
    return (x);
}

void samclient_free(t_samclient *x)
{
    int i;
    dsp_free((t_pxobject *)x);
    for (i = 0; i < x->channels; i++) {
        if (x->sigvec[i]) {
            sysmem_freeptr(x->sigvec[i]);
        }
    }
    if (x->sigvec) {
        sysmem_freeptr(x->sigvec);
    }
    if (x->sac != NULL) {
        delete x->sac;
    }
}

void samclient_assist(t_samclient *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        sprintf(s, "(signal) inlet %ld", a);
    }
    else { // outlet
        sprintf(s, "(int) 1 when SAM client connected, 0 when disconnected.");
    }
}

t_max_err samclient_port_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->port);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_port_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->port) {
            if (atom_getlong(argv) >= 1) {
                x->port = atom_getlong(argv);
            }
            else {
                x->port = 8000;
            }
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_type_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->type);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_type_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->type) {
            if (atom_getlong(argv) >= 0) {
                x->type = atom_getlong(argv);
            }
            else {
                x->type = 0;
            }
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_preset_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->preset);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_preset_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->preset) {
            if (atom_getlong(argv) >= 0) {
                x->preset = atom_getlong(argv);
            }
            else {
                x->preset = 0;
            }
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_x_pos_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->x_pos);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_x_pos_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->x_pos) {
            x->x_pos = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_y_pos_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->y_pos);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_y_pos_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->y_pos) {
            x->y_pos = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_width_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->width);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_width_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->width) {
            x->width = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_height_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->height);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_height_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->height) {
            x->height = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_depth_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->depth);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_depth_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->depth) {
            x->depth = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_pqsize_get(t_samclient *x, void *attr, long *argc, t_atom **argv)
{
	if (argc && argv) {
		char alloc;
		if (atom_alloc(argc, argv, &alloc)) {
			return MAX_ERR_GENERIC;
		}
		atom_setlong(*argv, x->pqsize);
	}
	return MAX_ERR_NONE;
}

t_max_err samclient_pqsize_set(t_samclient *x, void *attr, long argc, t_atom *argv)
{
	if (argc && argv) {
        if (atom_getlong(argv) != x->pqsize) {
            x->pqsize = atom_getlong(argv);
        }
	}
	return MAX_ERR_NONE;
}

void sac_create(t_samclient *x)
{
    int i;
    int err = sam::SAC_ERROR;
    sam::SacParams params;
    params.numChannels = (unsigned int)(x->channels);
    params.type = (sam::StreamingAudioType)(x->type);
    params.preset = x->preset;
    params.name = x->name->s_name;
    params.samIP = x->ip->s_name;
    params.samPort = (unsigned short)(x->port);
    params.payloadType = sam::PAYLOAD_PCM_16;
    params.packetQueueSize = x->pqsize;
    params.driveExternally = true;
    
    // SAC creation and initialization
    if (x->sac == NULL) {
        // default constructor (recommended)
        x->sac = new sam::StreamingAudioClient();
        // initialize SAC
        err = x->sac->init(params);
    }
    
    // start SAC
    if (err == sam::SAC_SUCCESS) {
        //object_post((t_object *)x, "Initialized SAC.");
        err = x->sac->start(x->x_pos, x->y_pos, x->width, x->height, x->depth, sam::SAC_DEFAULT_TIMEOUT);
        if (err != sam::SAC_SUCCESS) {
            object_error((t_object *)x, "Couldn't start SAC. Error: %d", err);
        }
    }
    else {
        object_error((t_object *)x, "Couldn't initialize SAC. Error: %d", err);
    }
    
    // check SAC sample rate and buffer size against Max/Pd
    if (err == sam::SAC_SUCCESS) {
        //object_post((t_object *)x, "Started SAC.");
        x->sam_samprate = (float)x->sac->getSampleRate();
        int p_sam_vecsize = x->sam_vecsize;
        x->sam_vecsize = (int)x->sac->getBufferSize();
        
        if (x->sam_samprate != x->samprate) {
            err = sam::SAC_ERROR;
            object_error((t_object *)x, "Sample rate mismatch. SAM sample rate: %f, SAC sample rate: %f", x->sam_samprate, x->samprate);
        }
        else if (x->sam_vecsize != x->vecsize) {
            if ((x->sam_vecsize > x->vecsize && (x->sam_vecsize % x->vecsize == 0)) ||
                (x->sam_vecsize < x->vecsize && (x->vecsize % x->sam_vecsize == 0))) {
                object_post((t_object *)x, "SAM buffer size: %d, SAC buffer size: %d", x->sam_vecsize, x->vecsize);
            }
            else {
                err = sam::SAC_ERROR;
                object_error((t_object *)x, "SAC buffer size (%d) is not factor or multiple of SAM buffer size (%d)", x->vecsize, x->sam_vecsize);
            }
        }
        
        if (err == sam::SAC_SUCCESS) {
            // resize x->sigvec, if necessary
            if (x->sam_vecsize != p_sam_vecsize) {
                for (i = 0; i < x->channels; i++) {
                    sysmem_resizeptrclear(x->sigvec[i], x->sam_vecsize * sizeof(float));
                }
            }
            x->offset = 0;
            x->sac_state = 1;
            outlet_int(x->m_outlet1, 1);
        }
        else {
            sac_destroy(x);
        }
    }
    else {
        sac_destroy(x);
    }
}

void sac_destroy(t_samclient *x)
{
    x->sac_state = 0;
    if (x->sac != NULL) {
        delete x->sac;
        x->sac = NULL;
        outlet_int(x->m_outlet1, 0);
    }
}

void samclient_dspstate(t_samclient *x, long n)
{
    // do something, if necessary
}

// this function is called when the DAC is enabled, and "registers" a function
// for the signal chain. in this case, "samclient_perform"
void samclient_dsp(t_samclient *x, t_signal **sp, short *count)
{
    //post("my sample rate is: %f", sp[0]->s_sr);
    int i;
    int n = sp[0]->s_n;
    float sr = sp[0]->s_sr;
    t_int **sigvec;
    int pointer_count;
    
    if (n != x->vecsize || sr != x->samprate) {
        x->vecsize = n;
        x->samprate = sr;
        
        if (x->sac_state == 1 && x->sac->isRunning()) {
            int err = sam::SAC_SUCCESS;
            
            if (x->sam_samprate != x->samprate) {
                err = sam::SAC_ERROR;
                object_error((t_object *)x, "Sample rate mismatch. SAM sample rate: %f, SAC sample rate: %f", x->sam_samprate, x->samprate);
            }
            else if (x->sam_vecsize != x->vecsize) {
                if ((x->sam_vecsize > x->vecsize && (x->sam_vecsize % x->vecsize == 0)) ||
                    (x->sam_vecsize < x->vecsize && (x->vecsize % x->sam_vecsize == 0))) {
                    object_post((t_object *)x, "SAM buffer size: %d, SAC buffer size: %d", x->sam_vecsize, x->vecsize);
                }
                else {
                    err = sam::SAC_ERROR;
                    object_error((t_object *)x, "SAC buffer size (%d) is not factor or multiple of SAM buffer size (%d)", x->vecsize, x->sam_vecsize);
                }
            }
            
            if (err == sam::SAC_ERROR) {
                sac_destroy(x);
            }
        }
    }
    
    pointer_count = 2 + x->channels; // object pointer + vector size + input channel
    
    sigvec = (t_int **) sysmem_newptr(pointer_count * sizeof(t_int *));
    for (i = 0; i < pointer_count; i++) {
        sigvec[i] = (t_int *) sysmem_newptrclear(sizeof(t_int));
    }
    
    sigvec[0] = (t_int *)x; // first pointer is to the object
    sigvec[1] = (t_int *)sp[0]->s_n; // second pointer is to vector size
    for (i = 2; i < pointer_count; i++) { // now attach the inlets
        sigvec[i] = (t_int *)sp[i-2]->s_vec;
    }
    
    //post("attached %d pointers", pointer_count);
    dsp_addv(samclient_perform, pointer_count, (void **) sigvec);
    sysmem_freeptr(sigvec);
}

// this is the Max 6 version of the dsp method -- it registers a function for the signal chain in Max 6,
// which operates on 64-bit audio signals.
void samclient_dsp64(t_samclient *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    int n = (int)maxvectorsize;
    float sr = (float)samplerate;
    
    if (n != x->vecsize || sr != x->samprate) {
        x->vecsize = n;
        x->samprate = sr;
        
        if (x->sac_state == 1 && x->sac->isRunning()) {
            int err = sam::SAC_SUCCESS;
            
            if (x->sam_samprate != x->samprate) {
                err = sam::SAC_ERROR;
                object_error((t_object *)x, "Sample rate mismatch. SAM sample rate: %f, SAC sample rate: %f", x->sam_samprate, x->samprate);
            }
            else if (x->sam_vecsize != x->vecsize) {
                if ((x->sam_vecsize > x->vecsize && (x->sam_vecsize % x->vecsize == 0)) ||
                    (x->sam_vecsize < x->vecsize && (x->vecsize % x->sam_vecsize == 0))) {
                    object_post((t_object *)x, "SAM buffer size: %d, SAC buffer size: %d", x->sam_vecsize, x->vecsize);
                }
                else {
                    err = sam::SAC_ERROR;
                    object_error((t_object *)x, "SAC buffer size (%d) is not factor or multiple of SAM buffer size (%d)", x->vecsize, x->sam_vecsize);
                }
            }
            
            if (err == sam::SAC_ERROR) {
                x->sac_state = 0;
                sac_destroy(x);
            }
        }
    }
    
	//post("my sample rate is: %f", samplerate);
	
	// instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
	// the dsp_add64 arguments are:
	// 1: the dsp64 object passed-in by the calling function
	// 2: a pointer to your object
	// 3: a pointer to your 64-bit perform method
	// 4: flags to alter how the signal chain handles your object -- just pass 0
	// 5: a generic pointer that you can use to pass any additional data to your perform method
    
	object_method(dsp64, gensym("dsp_add64"), x, samclient_perform64, 0, NULL);
}

t_int *samclient_perform(t_int *w)
{
    // DO NOT CALL post IN HERE, but you can call defer_low (not defer)
    
    // args are in a vector, sized as specified in samclient_dsp method
    // w[0] contains &samclient_perform, so we start at w[1]
    t_samclient *x = (t_samclient *)w[1];   // object instance pointer
    t_float *in;                            // pointer for audio at each inlet of the object
    int n = (int)w[2];                      // signal vector size
    int chans = (int)x->channels;           // number of input channels
    int i, j;
    
    if (x->sac_state == 1 && x->sac->isRunning()) {
        if (x->m_obj.z_disabled) {
            for (j = 0; j < n; j++, x->offset++) {
                // send x->sigvec to SAC
                if (x->offset >= x->sam_vecsize) {
                    x->sac->sendAudio(x->sigvec);
                    x->offset = 0;
                }
                // update x->sigvec
                for (i = 0; i < chans; i++) {
                    x->sigvec[i][x->offset] = 0.0f;
                }
            }
        }
        else {
            for (j = 0; j < n; j++, x->offset++) {
                // send x->sigvec to SAC
                if (x->offset >= x->sam_vecsize) {
                    x->sac->sendAudio(x->sigvec);
                    x->offset = 0;
                }
                // update x->sigvec
                for (i = 0; i < chans; i++) {
                    in = (t_float *)w[i+3];
                    x->sigvec[i][x->offset] = in[j];
                }
            }
        }
    }
    
    // you have to return the NEXT pointer in the array OR MAX WILL CRASH
    return (w + x->channels + 3);
}

// this is 64-bit perform method for Max 6
void samclient_perform64(t_samclient *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	//t_double *in;                 // we will get audio for each inlet of the object from the **ins argument
	int n = (int)sampleframes;      // signal vector size
    int chans = (int)x->channels;   // number of input channels
    int i, j;
    
    if (x->sac_state == 1 && x->sac->isRunning()) {
        if (x->m_obj.z_disabled) {
            for (j = 0; j < n; j++, x->offset++) {
                // send x->sigvec to SAC
                if (x->offset >= x->sam_vecsize) {
                    x->sac->sendAudio(x->sigvec);
                    x->offset = 0;
                }
                // update x->sigvec
                for (i = 0; i < chans; i++) {
                    x->sigvec[i][x->offset] = 0.0f;
                }
            }
        }
        else {
            for (j = 0; j < n; j++, x->offset++) {
                // send x->sigvec to SAC
                if (x->offset >= x->sam_vecsize) {
                    x->sac->sendAudio(x->sigvec);
                    x->offset = 0;
                }
                // update x->sigvec
                for (i = 0; i < chans; i++) {
                    x->sigvec[i][x->offset] = (float)(ins[i][j]);
                }
            }
        }
    }
}

#else
/*************************** Pd stuff ************************/
extern "C" void samclient_tilde_setup(void)
{
    samclient_class = class_new(gensym("samclient~"), (t_newmethod)samclient_new, (t_method)samclient_free, sizeof(t_samclient), 0, A_GIMME, 0);
    
	class_addmethod(samclient_class, (t_method)samclient_dsp, gensym("dsp"), A_CANT, 0);
    class_addmethod(samclient_class, (t_method)sac_connect, gensym("connect"), A_NULL, 0);
    class_addmethod(samclient_class, (t_method)sac_disconnect, gensym("disconnect"), A_NULL, 0);
    class_addmethod(samclient_class, (t_method)samclient_name, gensym("name"), A_SYMBOL, 0);
    class_addmethod(samclient_class, (t_method)samclient_ip, gensym("ip"), A_SYMBOL, 0);
    class_addmethod(samclient_class, (t_method)samclient_port, gensym("port"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_type, gensym("type"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_preset, gensym("preset"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_x_pos, gensym("x_pos"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_y_pos, gensym("y_pos"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_width, gensym("width"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_height, gensym("height"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_depth, gensym("depth"), A_FLOAT, 0);
    class_addmethod(samclient_class, (t_method)samclient_depth, gensym("pqsize"), A_FLOAT, 0);
    class_sethelpsymbol(samclient_class, gensym("samclient~-help"));
    
    CLASS_MAINSIGNALIN(samclient_class, t_samclient, x_f);
}

void *samclient_new(t_symbol *s, int argc, t_atom *argv)
{
    t_samclient *x = NULL;

    if ((x = (t_samclient *)pd_new(samclient_class))) {
        x->m_outlet1 = outlet_new(&x->m_obj, &s_float); // left outlet
        
        int i;
        
        // initialize class member defaults
        x->name = gensym("samclient");
        x->ip = gensym("127.0.0.1");
        x->port = 7770;
        x->channels = 1;
        x->type = 0;
        x->preset = 0;
        x->x_pos = 0;
        x->y_pos = 0;
        x->width = 0;
        x->height = 0;
        x->depth = 0;
        x->pqsize = -1;
        x->vecsize = sys_getblksize();
        x->sam_vecsize = x->vecsize;
        x->samprate = sys_getsr();
        x->sam_samprate = x->samprate;
        x->offset = 0;
        x->sac_state = 0;

        if (argc >= 1 && argv->a_type == A_FLOAT) {
            if (atom_getint(argv) >= 1) {
                x->channels = atom_getint(argv);
            }
        }
        
        // create additional signal inlets, if necessary
        for (i = 1; i < x->channels; i++) {
            signalinlet_new(&x->m_obj, x->x_f);
        }
                
        // allocate memory for arrays, initialize struct members
        x->sigvec = (float **) getbytes(x->channels * sizeof(float *));
        for (i = 0; i < x->channels; i++) {
            x->sigvec[i] = (float *) getbytes(x->sam_vecsize * sizeof(float));
        }
    }
    return (x);
}

void samclient_free(t_samclient *x)
{
    int i;
    for (i = 0; i < x->channels; i++) {
        if (x->sigvec[i]) {
            freebytes(x->sigvec[i], x->channels * sizeof(float));
        }
    }
    if (x->sigvec) {
        freebytes(x->sigvec, x->channels * sizeof(float *));
    }
    if (x->sac != NULL) {
        delete x->sac;
        x->sac = NULL;
    }
}

void sac_create(t_samclient *x)
{
    int i;
    int err = sam::SAC_ERROR;
    sam::SacParams params;
    params.numChannels = (unsigned int)(x->channels);
    params.type = (sam::StreamingAudioType)(x->type);
    params.preset = x->preset;
    params.name = x->name->s_name;
    params.samIP = x->ip->s_name;
    params.samPort = (unsigned short)(x->port);
    params.payloadType = sam::PAYLOAD_PCM_16;
    params.packetQueueSize = x->pqsize;
    params.driveExternally = true;
    
    // SAC creation and initialization
    if (x->sac == NULL) {
        // default constructor (recommended)
        x->sac = new sam::StreamingAudioClient();
        // initialize SAC
        err = x->sac->init(params);
    }
    
    // start SAC
    if (err == sam::SAC_SUCCESS) {
        //post("Initialized SAC.");
        err = x->sac->start(x->x_pos, x->y_pos, x->width, x->height, x->depth, sam::SAC_DEFAULT_TIMEOUT);
        if (err != sam::SAC_SUCCESS) {
            pd_error((t_object *)x, "Couldn't start SAC. Error: %d", err);
        }
    }
    else {
        pd_error((t_object *)x, "Couldn't initialize SAC. Error: %d", err);
    }
    
    // check SAC sample rate and buffer size against Max/Pd
    if (err == sam::SAC_SUCCESS) {
        //post("Started SAC.");
        x->sam_samprate = (float)x->sac->getSampleRate();
        int p_sam_vecsize = x->sam_vecsize;
        x->sam_vecsize = (int)x->sac->getBufferSize();
        
        if (x->sam_samprate != x->samprate) {
            err = sam::SAC_ERROR;
            pd_error((t_object *)x, "Sample rate mismatch. SAM sample rate: %f, SAC sample rate: %f", x->sam_samprate, x->samprate);
        }
        else if (x->sam_vecsize != x->vecsize) {
            if ((x->sam_vecsize > x->vecsize && (x->sam_vecsize % x->vecsize == 0)) ||
                (x->sam_vecsize < x->vecsize && (x->vecsize % x->sam_vecsize == 0))) {
                post("SAM buffer size: %d, SAC buffer size: %d", x->sam_vecsize, x->vecsize);
            }
            else {
                err = sam::SAC_ERROR;
                pd_error((t_object *)x, "SAC buffer size (%d) is not factor or multiple of SAM buffer size (%d)", x->vecsize, x->sam_vecsize);
            }
        }
        
        if (err == sam::SAC_SUCCESS) {
            // resize x->sigvec, if necessary
            if (x->sam_vecsize != p_sam_vecsize) {
                for (i = 0; i < x->channels; i++) {
                    x->sigvec[i] = (float *) resizebytes(x->sigvec[i], p_sam_vecsize * sizeof(float), x->sam_vecsize * sizeof(float));
                }
            }
            x->offset = 0;
            x->sac_state = 1;
            outlet_float((t_outlet *)(x->m_outlet1), 1);
        }
        else {
            sac_destroy(x);
        }
    }
    else {
        sac_destroy(x);
    }
}

void sac_destroy(t_samclient *x)
{
    x->sac_state = 0;
    if (x->sac!= NULL) {
        delete x->sac;
        x->sac = NULL;
        outlet_float((t_outlet *)x->m_outlet1, 0);
    }
}

void samclient_name(t_samclient *x, t_symbol *s)
{
    x->name = s;
}

void samclient_ip(t_samclient *x, t_symbol *s)
{
    x->ip = s;
}

void samclient_port(t_samclient *x, t_floatarg f)
{
    if (f >= 1.0) {
        x->port = (int)f;
    }
    else {
        x->port = 8000;
    }
}

void samclient_type(t_samclient *x, t_floatarg f)
{
    if (f >= 0) {
        x->type = (int)f;
    }
    else {
        x->type = 0;
    }
}

void samclient_preset(t_samclient *x, t_floatarg f)
{
    if (f >= 0) {
        x->preset = (int)f;
    }
    else {
        x->preset = 0;
    }
}

void samclient_x_pos(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->x_pos) {
        x->x_pos = (int)f;
    }
}

void samclient_y_pos(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->y_pos) {
        x->y_pos = (int)f;
    }
}

void samclient_width(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->width) {
        x->width = (int)f;
    }
}

void samclient_height(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->height) {
        x->height = (int)f;
    }
}

void samclient_depth(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->depth) {
        x->depth = (int)f;
    }
}

void samclient_pqsize(t_samclient *x, t_floatarg f)
{
    if ((int)f != x->pqsize) {
        x->pqsize = (int)f;
    }
}

// this function is called when the DAC is enabled, and "registers" a function
// for the signal chain. in this case, "samclient_perform"
void samclient_dsp(t_samclient *x, t_signal **sp)
{
    //post("my sample rate is: %f", sp[0]->s_sr);
    int i;
    int n = sp[0]->s_n;
    float sr = sp[0]->s_sr;
    t_int **sigvec;
    int pointer_count;
    
    if (n != x->vecsize || sr != x->samprate) {
        x->vecsize = n;
        x->samprate = sr;
        
        if (x->sac_state == 1 && x->sac->isRunning()) {
            int err = sam::SAC_SUCCESS;
            
            if (x->sam_samprate != x->samprate) {
                err = sam::SAC_ERROR;
                pd_error((t_object *)x, "Sample rate mismatch. SAM sample rate: %f, SAC sample rate: %f", x->sam_samprate, x->samprate);
            }
            else if (x->sam_vecsize != x->vecsize) {
                if ((x->sam_vecsize > x->vecsize && (x->sam_vecsize % x->vecsize == 0)) ||
                    (x->sam_vecsize < x->vecsize && (x->vecsize % x->sam_vecsize == 0))) {
                    post("SAM buffer size: %d, SAC buffer size: %d", x->sam_vecsize, x->vecsize);
                }
                else {
                    err = sam::SAC_ERROR;
                    pd_error((t_object *)x, "SAC buffer size (%d) is not factor or multiple of SAM buffer size (%d)", x->vecsize, x->sam_vecsize);
                }
            }
            
            if (err == sam::SAC_ERROR) {
                sac_destroy(x);
            }
        }
    }
    
    pointer_count = 2 + x->channels; // object pointer + vector size + input channel
    
    sigvec = (t_int **) getbytes(pointer_count * sizeof(t_int *));
    for (i = 0; i < pointer_count; i++) {
        sigvec[i] = (t_int *) getbytes(sizeof(t_int));
    }
    
    sigvec[0] = (t_int *)x; // first pointer is to the object
    sigvec[1] = (t_int *)sp[0]->s_n; // second pointer is to vector size
    for (i = 2; i < pointer_count; i++) { // now attach the inlets
        sigvec[i] = (t_int *)sp[i-2]->s_vec;
    }
    
    //post("attached %d pointers", pointer_count);
    dsp_addv(samclient_perform, pointer_count, (t_int *)sigvec);
    freebytes(sigvec, pointer_count * sizeof(t_int *));
}

t_int *samclient_perform(t_int *w)
{
    // DO NOT CALL post IN HERE, but you can call defer_low (not defer)
    
    // args are in a vector, sized as specified in samclient_dsp method
    // w[0] contains &samclient_perform, so we start at w[1]
    t_samclient *x = (t_samclient *)w[1];   // object instance pointer
    t_sample *in;                           // pointer for audio at each inlet of the object
    int n = (int)w[2];                      // signal vector size
    int chans = (int)x->channels;           // number of input channels
    int i, j;
    
    if (x->sac_state == 1 && x->sac->isRunning()) {
        for (j = 0; j < n; j++, x->offset++) {
            // send x->sigvec to SAC
            if (x->offset >= x->sam_vecsize) {
                x->sac->sendAudio(x->sigvec);
                x->offset = 0;
            }
            // update x->sigvec
            for (i = 0; i < chans; i++) {
                in = (t_sample *)w[i+3];
                x->sigvec[i][x->offset] = in[j];
            }
        }
    }
    
    // you have to return the NEXT pointer in the array OR MAX WILL CRASH
    return (w + x->channels + 3);
}
#endif

/*************************** Max & PD stuff ************************/

void sac_connect(t_samclient *x)
{
    if (x->sac_state == 0) {
        sac_create(x);
    }
}

void sac_disconnect(t_samclient *x)
{
    if (x->sac_state == 1) {
        sac_destroy(x);
    }
}
