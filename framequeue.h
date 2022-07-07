struct Frame {
    int location;
    struct Frame *next;
};

struct Frame *construct_FQ(int loc);
struct Frame *insertFrame(struct Frame *head, struct Frame *newFrame);